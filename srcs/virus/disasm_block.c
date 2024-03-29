#include "blocks.h"
#include "accessors.h"
#include "disasm.h"
#include "utils.h"
#include "errors.h"
#include "debug.h"

/*
** sort_by_label_addr
** - sorts array inline
** - orders sidecar like array as a side effect
*/
static void	sort_by_label_addr(struct jump *sidecar[MAX_JUMPS],
			struct control_flow array[MAX_JUMPS], size_t len)
{
	if (len < 2) return;

	const void	*pivot = array[len / 2].label_addr;
	int		i, j;

	for (i = 0, j = len - 1; ; i++, j--)
	{
		while (array[i].label_addr < pivot) i++;
		while (array[j].label_addr > pivot) j--;

		if (i >= j) break;

		// swap array inline
		struct control_flow	temp = array[i];
		array[i] = array[j];
		array[j] = temp;
		// swap sidecar in the same manner
		struct jump		*side_temp = sidecar[i];
		sidecar[i] = sidecar[j];
		sidecar[j] = side_temp;
	}
	sort_by_label_addr(sidecar, array, i);
	sort_by_label_addr(sidecar + i, array + i, len - i);
}

static void	assign_labels(struct label labels[MAX_JUMPS],
			struct jump *label_origins[MAX_JUMPS],
			struct code_block *block,
			struct control_flow jumps_info[MAX_JUMPS], size_t njumps)
{
	sort_by_label_addr(label_origins, jumps_info, njumps);

	struct label	*current_label = labels;
	current_label->location = jumps_info[0].label_addr;
	current_label->jumps    = &label_origins[0];

	block->nlabels++;

	for (size_t i = 0; i < njumps; i++)
	{
		label_origins[i]->location = jumps_info[i].addr;

		if (jumps_info[i].label_addr == current_label->location)
		{
			current_label->njumps++;
			continue;
		}
		current_label++;
		block->nlabels++;

		current_label->location = jumps_info[i].label_addr;
		current_label->jumps    = &label_origins[i];
		current_label->njumps   = 1;
	}
	block->labels = labels;
}

static void	assign_jumps(struct jump jumps[MAX_JUMPS], struct code_block *block,
			const struct control_flow jumps_info[MAX_JUMPS], size_t njumps)
{
	for (size_t i = 0; i < njumps; i++)
	{
		jumps[i].location = jumps_info[i].addr;
		jumps[i].instruction_size = jumps_info[i].length;
		jumps[i].value_addr = jumps_info[i].value_addr;
		jumps[i].value_size = jumps_info[i].value_length;
		jumps[i].value = jumps_info[i].value;
	}
	block->jumps  = jumps;
	block->njumps = njumps;
}

bool		disasm_block(struct block_allocation *block_alloc,
			struct safe_ptr input_code)
{
	struct control_flow	jumps_info[MAX_JUMPS];
	void			*code = input_code.ptr;
	size_t			size  = input_code.size;

	size_t	njumps = disasm_jumps(jumps_info, MAX_JUMPS, code, size);

	if (njumps == 0 || njumps > MAX_JUMPS)
		return errors(ERR_THROW, _ERR_NO, _ERR_T_DISASM_BLOCK);

	bzero(block_alloc, sizeof(*block_alloc));
	block_alloc->blocks[0].ref.ptr  = code;
	block_alloc->blocks[0].ref.size = size;

	assign_jumps(block_alloc->jumps, &block_alloc->blocks[0], jumps_info, njumps);

	for (size_t i = 0; i < njumps; i++)
		block_alloc->label_origins[i] = &block_alloc->jumps[i];

	assign_labels(block_alloc->labels, block_alloc->label_origins, &block_alloc->blocks[0], jumps_info, njumps);
	debug_print_original_block(block_alloc->blocks);

	return true;
}
