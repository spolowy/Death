#ifndef DEBUG_H
# define DEBUG_H

# ifdef DEBUG_BLOCKS
void	debug_print_original_block(const struct code_block *blocks);
void	debug_print_split_blocks(const struct code_block *blocks, size_t nblocks, struct safe_ptr input_code, struct safe_ptr output_buffer);
void	debug_print_general_block(struct safe_ptr input_code, struct safe_ptr output_buffer, size_t entry_point, int32_t virus_func_shift, size_t output_size, uint64_t seed);
# else
#  define debug_print_original_block(...)
#  define debug_print_split_blocks(...)
#  define debug_print_general_block(...)
# endif

# ifdef DEBUG_OPERANDS
void	debug_print_operands(const struct operand *instructions, size_t n);
# else
#  define debug_print_operands(...)
# endif

#endif
