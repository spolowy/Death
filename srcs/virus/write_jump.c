#include <stdint.h>
#include <stdbool.h>
#include "disasm.h"
#include "bytes.h"

void		write_jump(void *buffer, int32_t value, uint8_t value_size)
{
	if (value_size == DWORD)     *(int32_t*)buffer = (int32_t)(value);
	else if (value_size == WORD) *(int16_t*)buffer = (int16_t)(value);
	else if (value_size == BYTE) *(int8_t*)buffer  = (int8_t)(value);
}

void		write_trailing_jump(void *buffer, int32_t value)
{
	uint8_t	*opcode = (uint8_t*)(buffer);
	int32_t	*arg    = (int32_t*)(buffer + 1);

	*opcode = 0xe9;
	*arg    = value;
}
