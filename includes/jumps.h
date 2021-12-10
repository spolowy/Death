#ifndef JUMPS_H
# define JUMPS_H

# define JMP32_INST_SIZE	5
# define CALL32_INST_SIZE	5

# define is_jmp32(x)	(x == 0xe9)

bool	write_jmp32(struct safe_ptr ref, size_t offset, int32_t value);
bool	write_i32_value(struct safe_ptr ref, size_t offset, int32_t value);
void	*find_first_jmp32(struct safe_ptr ref, size_t offset);
void	*get_jmp32_destination(struct safe_ptr ref, size_t offset);

#endif
