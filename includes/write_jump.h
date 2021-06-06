#ifndef WRITE_JUMP_H
# define WRITE_JUMP_H

# include <stdint.h>

# define JUMP32_INST_SIZE	5

void		write_jump(void *buffer, int32_t value, uint8_t value_size);
void		write_trailing_jump(void *buffer, int32_t value);

#endif
