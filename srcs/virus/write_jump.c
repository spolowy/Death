#include <stdint.h>
#include <stdbool.h>
#include "disasm.h"
#include "compiler_utils.h"

void		write_jump_arg(void *arg, int32_t value, uint8_t value_size)
{
	if (value_size == DWORD)     *(int32_t*)arg = (int32_t)(value);
	else if (value_size == WORD) *(int16_t*)arg = (int16_t)(value);
	else if (value_size == BYTE) *(int8_t*)arg  = (int8_t)(value);
}

void		write_jump(void *buffer, int32_t value, uint8_t value_size)
{
	uint8_t	*opcode = (uint8_t*)(buffer);
	int32_t	*arg    = (int32_t*)(buffer + 1);

	*opcode = 0xe9; // rel16/32
	write_jump_arg(arg, value, value_size);
}
