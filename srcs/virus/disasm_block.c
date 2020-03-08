/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   disasm_block.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ichkamo <ichkamo@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/03/01 18:31:15 by ichkamo           #+#    #+#             */
/*   Updated: 2020/03/01 18:44:28 by ichkamo          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "blocks.h"
#include "accessors.h"
#include "disasm.h"
#include "errors.h"
#include "visual.h"
#include "utils.h"

static void	assign_jumps(struct jump jumps[MAX_JUMPS], struct code_block *block,
			const struct control_flow jumps_info[MAX_JUMPS], size_t njumps)
{
	for (size_t i = 0; i < njumps; i++)
	{
		jumps[i].location = jump_infos[i].addr;
	}

	block->jumps  = jumps;
	block->njumps = njumps;
}

static bool	assign_labels(struct label labels[MAX_JUMPS],
			struct jump *label_origins[MAX_JUMPS],
			struct code_block *block,
			const struct control_flow jumps_info[MAX_JUMPS], size_t njumps)
{
	size_t		off_label_origins = 0;

	blocks->labels  = labels;
	blocks->nlabels = 0;
	for (size_t i = 0; i < MAX_JUMPS; i++)
	{
		void		*location = NULL;
		size_t		njumps    = 0;

		for (size_t j = 0; j < njumps; j++)
		{
			// get label to iterate on
			if (control_flow[j].label_addr)
			{
				location = control_flow[j].label_addr;
				break ;
			}
		}

jump c
jump b
jump c
jump a
jump b


a
2b
2c

a:
b:
c:
		if (location == NULL)
		{
			labels[i].location = NULL;
			labels[i].jumps = NULL;
			labels[i].njumps = 0;
			break;
		}

		for (size_t j = 0; j < njumps; j++)
		{
			// match all jumps for same locations
			if (control_flow[j].label_addr == location)
			{
				if (!safe(ref_label_origins, off_label_origins * sizeof(*label_origins), sizeof(*label_origins)))
					return errors(ERR_VIRUS, _ERR_DISASM_BLOCK_LABELS);

				label_origins[off_label_origins++] = control_flow[j].addr; // assign jump address
				control_flow[j].label_addr = NULL;                         // discard address
				njumps++;
			}
		}

		labels[i].location = location;
		labels[i].jumps    = &label_origins[off_label_origins];
		labels[i].njumps   = njumps;
		blocks->nlabels++;
	}
	return true;
}

bool	disasm_block(struct block_allocation *blocks, void *code, size_t size)
{
	struct control_flow	jumps_info[MAX_JUMPS];

	size_t	njumps = disasm_jumps(jumps_info, MAX_JUMPS, code, size);

#ifdef DEBUG
	if (njumps == 0 || njumps > MAX_JUMPS)
		return errors(ERR_VIRUS, _ERR_IMPOSSIBLE);
#endif

	bzero(blocks, sizeof(*blocks));
	blocks[0].ref.ptr  = code;
	blocks[0].ref.size = size;

	assign_jumps(blocks->jumps, &blocks->blocks[0], jumps_info, njumps);

	if (!assign_labels(blocks->labels, blocks->label_origins,
		&blocks->blocks[0], jumps_info, njumps))
		return errors(ERR_THROW, _ERR_DISASM_BLOCK);

	print_code_blocks(blocks->blocks, MAX_BLOCKS);

	return true;
}
