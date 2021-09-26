#ifndef JUMPS_H
# define JUMPS_H

# define JUMP32_INST_SIZE	5
# define CALL32_INST_SIZE	5

void	write_jump_arg(void *arg, int32_t value, uint8_t value_size);
void	write_jump(void *buffer, int32_t value, uint8_t value_size);

bool	is_jump32(const uint8_t *code);
void	*find_first_jump(struct safe_ptr ref, size_t offset);
void	*get_jump_destination(void *jump_ptr);

#endif
