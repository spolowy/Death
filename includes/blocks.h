#ifndef BLOCKS_H
# define BLOCKS_H

# include <stdbool.h>
# include <stdint.h>
# include <stddef.h>
# include <sys/types.h>

# include "utils.h"
# include "accessors.h"

# define NSHUFFLE		42
# define NDIVISIONS		8
# define NBLOCKS		POW2(NDIVISIONS)
# define MAX_JUMPS		4096

struct				jump
{
	void			*location;       // original code
	uint8_t			instruction_size;// (call, jmp) instruction size
	void			*value_addr;     // instruction value address
	uint8_t			value_size;      // value size in byte
	int32_t			value;           // jump distance
	int32_t			value_shift;     // amount to shift
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
	int32_t			shift_amount;    // amount to shift
	struct code_block	*trailing_block; // trailing block (if any)
};

struct				block_allocation
{
	struct code_block	blocks[NBLOCKS];
	struct jump		jumps[MAX_JUMPS];
	struct label		labels[MAX_JUMPS];
	struct jump		*label_origins[MAX_JUMPS];
};

bool		disasm_block(struct block_allocation *block_alloc, struct safe_ptr input_code);

#endif
