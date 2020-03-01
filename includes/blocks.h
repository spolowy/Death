/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   blocks.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ichkamo <ichkamo@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/03/01 17:27:07 by ichkamo           #+#    #+#             */
/*   Updated: 2020/03/01 18:49:04 by ichkamo          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef BLOCKS_H
# define BLOCKS_H

# include <stdbool.h>
# include <stdint.h>
# include <stddef.h>
# include <sys/types.h>

# include "accessors.h"

# define MAX_BLOCKS		128
# define MAX_JUMPS		1024

struct				jump
{
	void			*location;       // original code
};

struct				label
{
	void			*location;       // original code
	struct jump		**jumps;         // jumps going to label
	size_t			njumps;          // number of jumps (> 0)
};

struct				code_block
{
	struct safe_ptr		ref;             // original code memory
	struct label		*labels;         // labels in code (if any)
	size_t			nlabels;         // number of labels
	struct jump		*jumps;          // jumps in code (if any)
	size_t			njumps;          // number of jumps
	int			shift_amount;    // amount to shift
	struct code_block	*trailing_block; // trailing block (if any)
};

struct				block_allocation
{
	struct	code_block	blocks[MAX_BLOCKS];
	struct	jump		jumps[MAX_JUMPS];
	struct	label		labels[MAX_JUMPS];
	struct	jump		*label_origins[MAX_JUMPS];
};

bool	disasm_block(struct block_allocation *blocks, void *code, size_t size);

#endif
