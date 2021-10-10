#include <stddef.h>
#include "accessors.h"
#include "disasm_utils.h"
#include "errors.h"
#include "utils.h"

bool	known_instruction(const void *code, size_t codelen)
{
	uint32_t	table_opcode[TABLESIZE];
				    /* 0 1 2 3 4 5 6 7  8 9 a b c d e f */
	table_opcode[0]    = BITMASK32(1,1,1,1,1,1,0,0, 1,1,1,1,1,1,0,0,  /* 0 */
				       1,1,1,1,1,1,0,0, 1,1,1,1,1,1,0,0); /* 1 */
	table_opcode[1]    = BITMASK32(1,1,1,1,1,1,0,0, 1,1,1,1,1,1,0,0,  /* 2 */
				       1,1,1,1,1,1,0,0, 1,1,1,1,1,1,0,0); /* 3 */
	table_opcode[2]    = BITMASK32(0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  /* 4 */
				       1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1); /* 5 */
	table_opcode[3]    = BITMASK32(0,0,1,1,0,0,0,0, 1,1,1,1,0,0,0,0,  /* 6 */
				       1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1); /* 7 */
	table_opcode[4]    = BITMASK32(1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,  /* 8 */
				       0,0,0,0,0,0,0,0, 0,1,0,0,0,0,0,0); /* 9 */
	table_opcode[5]    = BITMASK32(1,1,1,1,0,0,0,0, 1,1,0,0,0,0,0,0,  /* a */
				       1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1); /* b */
	table_opcode[6]    = BITMASK32(1,1,1,0,1,1,1,1, 1,0,1,0,0,1,0,0,  /* c */
				       1,1,1,1,1,1,0,0, 1,1,1,1,1,1,1,1); /* d */
	table_opcode[7]    = BITMASK32(1,1,1,1,1,1,1,1, 1,1,0,1,0,0,0,0,  /* e */
				       0,0,0,0,0,0,1,1, 0,0,0,0,0,0,1,1); /* f */

	uint32_t	table_opcode_0f[TABLESIZE];
				    /* 0 1 2 3 4 5 6 7  8 9 a b c d e f */
	table_opcode_0f[0] = BITMASK32(1,1,1,1,0,0,0,0, 0,0,0,0,0,0,0,0,  /* 0 */
				       1,1,1,1,1,1,1,1, 1,0,0,0,0,0,0,1); /* 1 */
	table_opcode_0f[1] = BITMASK32(1,1,1,1,0,0,0,0, 1,1,1,1,1,1,1,1,  /* 2 */
				       0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0); /* 3 */
	table_opcode_0f[2] = BITMASK32(1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,  /* 4 */
				       1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1); /* 5 */
	table_opcode_0f[3] = BITMASK32(1,1,1,1,1,1,1,1, 1,1,1,1,0,0,1,1,  /* 6 */
				       1,1,1,1,1,1,1,0, 1,1,0,0,0,0,1,1); /* 7 */
	table_opcode_0f[4] = BITMASK32(1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,  /* 8 */
				       1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1); /* 9 */
	table_opcode_0f[5] = BITMASK32(0,0,0,1,1,1,0,0, 0,0,0,1,1,1,1,1,  /* a */
				       1,1,1,1,1,1,1,1, 0,1,1,1,1,1,1,1); /* b */
	table_opcode_0f[6] = BITMASK32(1,1,1,1,1,1,1,1, 0,0,0,0,0,0,0,0,  /* c */
				       0,1,1,1,1,1,0,1, 1,1,1,1,1,1,1,1); /* d */
	table_opcode_0f[7] = BITMASK32(1,1,1,1,1,1,0,1, 1,1,1,1,1,1,1,1,  /* e */
				       0,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,0); /* f */

	uint8_t		*p = (uint8_t*)code;
	uint8_t		opcode;                 // current opcode
	int8_t		prefix = 0;             // opcode prefix(es)

next_opcode:

	if (!codelen--) // error if instruction is too long
	{
		hexdump_text(code, INSTRUCTION_MAXLEN, (size_t)p - (size_t)code);
		goto error;
	}
	opcode = *p++;

	// NULL byte prefixes
	if ((opcode == 0x26 || opcode == 0x2e
	||   opcode == 0x36 || opcode == 0x3e
	// FS/GS segment override prefixes
	||   opcode == 0x64 || opcode == 0x65
	// REX prefixes
	||  (opcode >= 0x40 && opcode <= 0x4f)
	// assert LOCK# signal prefix
	||   opcode == 0xf0)
	&&  !(prefix))
		{goto next_opcode;}
	// mandatory prefix for two-byte opcode
	else if (opcode == 0x0f) {prefix |= OP_PREFIX_0F; goto next_opcode;}

	return CHECK_TABLE(table_opcode, opcode) || CHECK_TABLE(table_opcode_0f, opcode);
error:
	return errors(ERR_VIRUS, _ERR_V_INSTRUCTION_LENGTH);
}

void	*step_instruction(const void *code, size_t codelen)
{
	const size_t	instruction_length = disasm_length(code, codelen);

	if (instruction_length == 0)
	{
		errors(ERR_THROW, _ERR_T_STEP_INSTRUCTION);
		return NULL;
	}

	void		*rip = NULL;
	uint8_t		*p = (uint8_t*)code;
	uint8_t		opcode;
	uint8_t		prefix = 0;

next_opcode:

	opcode = *p++;

	// check if has prefix
	if (opcode == 0x0f) {prefix |= OP_PREFIX_0F; goto next_opcode;}

	// rel8
	if ((opcode >= 0x70 && opcode <= 0x7f)           // JMPcc
	|| (opcode >= 0xe0 && opcode <= 0xe3)            // LOOP/LOOPcc/JMPcc
	|| (opcode == 0xeb))                             // JMP
	{
		int8_t	jump_value = *((int8_t*)p);
		rip = ((void*)code + instruction_length) + jump_value;
	}
	// rel16/32
	else if ((opcode == 0xe8)                          // CALL
	|| (opcode == 0xe9)                                // JMP
	|| (prefix && (opcode >= 0x80 && opcode <= 0x8f))) // JMPcc
	{
		int32_t	jump_value = *((int32_t*)p);
		rip = ((void*)code + instruction_length) + jump_value;
	}
	else
	{
		rip = (void*)code + instruction_length;
	}
	return rip;
}
