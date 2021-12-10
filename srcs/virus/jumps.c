#include <stdint.h>
#include <stdbool.h>
#include "disasm.h"
#include "jumps.h"
#include "compiler_utils.h"
#include "errors.h"

bool	write_jmp32(struct safe_ptr ref, size_t offset, int32_t value)
{
	uint8_t	*jump_opcode = safe(ref, offset, JMP32_INST_SIZE);
	int32_t	*jump_value  = (int32_t*)(jump_opcode + 1);

	if (jump_opcode == NULL)
		return errors(ERR_FILE, _ERR_F_READ_JUMP, _ERR_T_WRITE_JMP32);

	*jump_opcode = 0xe9;
	*jump_value  = value;

	return true;
}

bool	write_i32_value(struct safe_ptr ref, size_t offset, int32_t value)
{
	void	*location = safe(ref, offset, DWORD);

	if (location == NULL)
		return errors(ERR_FILE, _ERR_F_READ_JUMP, _ERR_T_WRITE_I32_VALUE);

	*(int32_t*)(location) = value;

	return true;
}

void	*find_first_jmp32(struct safe_ptr ref, size_t offset)
{
	uint8_t	*code = safe(ref, offset, INSTRUCTION_MAXLEN);

	while (true)
	{
		if (code == NULL)
		{
			errors(ERR_FILE, _ERR_F_READ_LOADER, _ERR_T_WRITE_JMP32);
			goto error;
		}
		if (!known_instruction(code, INSTRUCTION_MAXLEN))
		{
			goto error;
		}
		if (is_jmp32(*code))
			return code;

		code = step_instruction(code, INSTRUCTION_MAXLEN);
	}
error:
	return NULL;
}

void	*get_jmp32_destination(struct safe_ptr ref, size_t offset)
{
	uint8_t	*jump_opcode = safe(ref, offset, JMP32_INST_SIZE);
	int32_t	*jump_value  = (int32_t*)(jump_opcode + 1);

	if (jump_opcode == NULL)
	{
		errors(ERR_FILE, _ERR_F_READ_JUMP, _ERR_T_GET_JMP32_DESTINATION);
		return NULL;
	}
	return ((jump_opcode + JMP32_INST_SIZE) + *jump_value);
}
