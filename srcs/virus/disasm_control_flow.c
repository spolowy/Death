
#include <unistd.h>
#include <stdio.h>
#include "disasm_utils.h"

/* sizes in byte */
# define BYTE		1
# define WORD		2
# define DWORD		4
# define PWORD		6
# define QWORD		8
# define TWORD		10

/*
Jc(onditionnal)c(ode) notes:
	- The terms “less” and “greater” are used for comparisons of signed integers.
	- The terms “above” and “below” are used for unsigned integers.
	- REX is used by default with Jcc.
*/

/*
** return true if successfully disassembled a control flow instruction
** return false if failed to disassembled a control flow instruction
*/

static bool	disasm_instruction(void **value_addr, int32_t *value, int8_t *value_length,
			const void *code, size_t codelen)
{
	uint8_t		*p = (uint8_t*)code;
	int8_t		prefix = 0;          /* prefix(es) */
	uint8_t		opcode;

	*value_addr = NULL;
	*value = 0;

	/* set <codelen> to INSTRUCTION_MAXLEN if it exceeds it */
	if (codelen > INSTRUCTION_MAXLEN) codelen = INSTRUCTION_MAXLEN;

	/* prefix loop */
	next_opcode:

	if (!codelen--) return false; /* error if instruction is too long */
	opcode = *p++;

	/* check if has prefix */
	if (opcode == 0x0f) {prefix |= OP_PREFIX_0F; goto next_opcode;}

	// jmp

	// jcc
	// call
	// loopcc
	// loop

	/* rel8 */
	if ((opcode >= 0x70 && opcode <= 0x7f) /* JMPcc             */
	|| (opcode >= 0xe0 && opcode <= 0xe3)  /* LOOP/LOOPcc/JMPcc */
	|| (opcode == 0xeb))                   /* JMP               */
	{
		*value_addr = p;
		*value = *((int8_t*)p);
		*value_length = BYTE;
	}
	/* rel16/32 */
	else if ((opcode == 0xe8)                          /* CALL  */
	|| (opcode == 0xe9)                                /* JMP   */
	|| (prefix && (opcode >= 0x80 && opcode <= 0x8f))) /* JMPcc */
	{
		*value_addr = p;
		*value = *((int32_t*)p);
		*value_length = DWORD;
	}

	return (*value_addr && *value && *value_length);
}

size_t		disasm_control_flow(struct control_flow *buf, size_t buflen,
			const void *code, size_t codelen)
{
	void			*p_code = (void*)code;
	struct control_flow	*p_buf  = buf;
	size_t			instruction_length = 0;

	while (codelen && buflen)
	{
		instruction_length = disasm_length(p_code, codelen);
		if (instruction_length == 0) return 0;

		if (disasm_instruction(&p_buf->value_addr, &p_buf->value, &p_buf->value_length,
			p_code, codelen))
		{
			p_buf->addr       = p_code;
			p_buf->length     = instruction_length;
			p_buf->label_addr = p_buf->addr + instruction_length + p_buf->value;
			p_buf  += 1;
			buflen -= 1;
		}
		p_code  += instruction_length;
		codelen -= instruction_length;
	}

	/* number of control flow instructions successfully disassembled */
	return p_buf - buf;
}
