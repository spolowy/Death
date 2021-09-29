#ifndef JUMPS_H
# define JUMPS_H

# define JUMP32_INST_SIZE	5
# define CALL32_INST_SIZE	5

# define is_jump32(x)	(x == 0xe9)

bool	write_jump32(struct safe_ptr ref, size_t offset, int32_t value);
bool	write_jump32_value(struct safe_ptr ref, size_t offset, int32_t value);
void	*find_first_jump32(struct safe_ptr ref, size_t offset);
void	*get_jump32_destination(struct safe_ptr ref, size_t offset);

#endif
