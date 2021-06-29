
#include "disasm_utils.h"
#include "bytes.h"
#include "errors.h"

/* opcode flags  */
# define MODRM		(1 << 0)            /* MODRM byte       */
# define TEST_F6	(1 << 1)            /* <test> exception */
# define TEST_F7	(1 << 2)            /* <test> exception */
# define TEST		(TEST_F6 | TEST_F7)

/*
** Disassemble an instruction pointed by <code> for a maximum
** length of <codelen> and return the size of said instruction.
** Returns a value between 1 and 15 included if successful.
** Returns 0 if failed.
**
** #_modrm:         possess a modrm byte.
** #_imm8/16/32/64: possess an immediate value of BYTE/WORD/DWORD/QWORD size.
** #_mem16/32:      possess a direct memory offset of WORD/DWORD size.
**
** /!\
** Merged immediate 16/32:
** Tables for 16 and 32-bit immediate values are merged for the sake of space
** and performance.
** But technically some opcode take only a WORD such as 0xc3 (<retn>).
** In this case if an operand size modifier is present, it would lead to an
** incorrect result.
**
** The <test> exception:
** 0xf6 (<test> imm8) and 0xf7 (<test> imm16/32) both share their opcode with
** other instructions (differentiated by their opcode extension in modrm.reg).
** The fact that the same primary opcode got so many length variation makes
** them an exception.
** /!\
*/
uint8_t		disasm_length(const void *code, size_t codelen)
{

/* -------------------------------- <opcode> -------------------------------- */

	uint32_t	table_opcode_modrm[TABLESIZE];
					          /* 0 1 2 3 4 5 6 7  8 9 a b c d e f */
	table_opcode_modrm[0]            = BITMASK32(1,1,1,1,0,0,0,0, 1,1,1,1,0,0,0,0,  /* 0 */
					             1,1,1,1,0,0,0,0, 1,1,1,1,0,0,0,0); /* 1 */
	table_opcode_modrm[1]            = BITMASK32(1,1,1,1,0,0,0,0, 1,1,1,1,0,0,0,0,  /* 2 */
					             1,1,1,1,0,0,0,0, 1,1,1,1,0,0,0,0); /* 3 */
	table_opcode_modrm[2]            = BITMASK32(0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  /* 4 */
					             0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0); /* 5 */
	table_opcode_modrm[3]            = BITMASK32(0,0,1,1,0,0,0,0, 0,1,0,1,0,0,0,0,  /* 6 */
					             0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0); /* 7 */
	table_opcode_modrm[4]            = BITMASK32(1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,  /* 8 */
					             0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0); /* 9 */
	table_opcode_modrm[5]            = BITMASK32(0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  /* a */
					             0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0); /* b */
	table_opcode_modrm[6]            = BITMASK32(1,1,0,0,1,1,1,1, 0,0,0,0,0,0,0,0,  /* c */
					             1,1,1,1,0,0,0,0, 1,1,1,1,1,1,1,1); /* d */
	table_opcode_modrm[7]            = BITMASK32(0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  /* e */
					             0,0,0,0,0,0,1,1, 0,0,0,0,0,0,1,1); /* f */
	uint32_t	table_opcode_imm8[TABLESIZE];
				                  /* 0 1 2 3 4 5 6 7  8 9 a b c d e f */
	table_opcode_imm8[0]             = BITMASK32(0,0,0,0,1,0,0,0, 0,0,0,0,1,0,0,0,  /* 0 */
					             0,0,0,0,1,0,0,0, 0,0,0,0,1,0,0,0); /* 1 */
	table_opcode_imm8[1]             = BITMASK32(0,0,0,0,1,0,0,0, 0,0,0,0,1,0,0,0,  /* 2 */
					             0,0,0,0,1,0,0,0, 0,0,0,0,1,0,0,0); /* 3 */
	table_opcode_imm8[2]             = BITMASK32(0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  /* 4 */
					             0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0); /* 5 */
	table_opcode_imm8[3]             = BITMASK32(0,0,0,0,0,0,0,0, 0,0,1,1,0,0,0,0,  /* 6 */
					             1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1); /* 7 */
	table_opcode_imm8[4]             = BITMASK32(1,0,1,1,0,0,0,0, 0,0,0,0,0,0,0,0,  /* 8 */
					             0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0); /* 9 */
	table_opcode_imm8[5]             = BITMASK32(0,0,0,0,0,0,0,0, 1,0,0,0,0,0,0,0,  /* a */
					             1,1,1,1,1,1,1,1, 0,0,0,0,0,0,0,0); /* b */
	table_opcode_imm8[6]             = BITMASK32(1,1,0,0,0,0,1,0, 1,0,0,0,0,1,0,0,  /* c */
					             0,0,0,0,1,1,0,0, 0,0,0,0,0,0,0,0); /* d */
	table_opcode_imm8[7]             = BITMASK32(1,1,1,1,1,1,1,1, 0,0,0,1,0,0,0,0,  /* e */
					             0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0); /* f */
	uint32_t	table_opcode_imm16_32[TABLESIZE];
				                  /* 0 1 2 3 4 5 6 7  8 9 a b c d e f */
	table_opcode_imm16_32[0]         = BITMASK32(0,0,0,0,0,1,0,0, 0,0,0,0,0,1,0,0,  /* 0 */
					             0,0,0,0,0,1,0,0, 0,0,0,0,0,1,0,0); /* 1 */
	table_opcode_imm16_32[1]         = BITMASK32(0,0,0,0,0,1,0,0, 0,0,0,0,0,1,0,0,  /* 2 */
					             0,0,0,0,0,1,0,0, 0,0,0,0,0,1,0,0); /* 3 */
	table_opcode_imm16_32[2]         = BITMASK32(0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  /* 4 */
					             0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0); /* 5 */
	table_opcode_imm16_32[3]         = BITMASK32(0,0,0,0,0,0,0,0, 1,1,0,0,0,0,0,0,  /* 6 */
					             0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0); /* 7 */
	table_opcode_imm16_32[4]         = BITMASK32(0,1,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  /* 8 */
					             0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0); /* 9 */
	table_opcode_imm16_32[5]         = BITMASK32(0,0,0,0,0,0,0,0, 0,1,0,0,0,0,0,0,  /* a */
					             0,0,0,0,0,0,0,0, 1,1,1,1,1,1,1,1); /* b */
	table_opcode_imm16_32[6]         = BITMASK32(0,0,1,0,0,0,0,1, 1,0,1,0,0,0,0,0,  /* c */
					             0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0); /* d */
	table_opcode_imm16_32[7]         = BITMASK32(0,0,0,0,0,0,0,0, 1,1,0,0,0,0,0,0,  /* e */
					             0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0); /* f */
	uint32_t	table_opcode_imm64[TABLESIZE];
					          /* 0 1 2 3 4 5 6 7  8 9 a b c d e f */
	table_opcode_imm64[0]            = BITMASK32(0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  /* 0 */
					             0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0); /* 1 */
	table_opcode_imm64[1]            = BITMASK32(0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  /* 2 */
					             0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0); /* 3 */
	table_opcode_imm64[2]            = BITMASK32(0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  /* 4 */
					             0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0); /* 5 */
	table_opcode_imm64[3]            = BITMASK32(0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  /* 6 */
					             0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0); /* 7 */
	table_opcode_imm64[4]            = BITMASK32(0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  /* 8 */
					             0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0); /* 9 */
	table_opcode_imm64[5]            = BITMASK32(0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  /* a */
					             0,0,0,0,0,0,0,0, 1,1,1,1,1,1,1,1); /* b */
	table_opcode_imm64[6]            = BITMASK32(0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  /* c */
					             0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0); /* d */
	table_opcode_imm64[7]            = BITMASK32(0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  /* e */
					             0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0); /* f */
	uint32_t	table_opcode_mem16_32[TABLESIZE];
					          /* 0 1 2 3 4 5 6 7  8 9 a b c d e f */
	table_opcode_mem16_32[0]         = BITMASK32(0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  /* 0 */
					             0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0); /* 1 */
	table_opcode_mem16_32[1]         = BITMASK32(0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  /* 2 */
					             0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0); /* 3 */
	table_opcode_mem16_32[2]         = BITMASK32(0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  /* 4 */
					             0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0); /* 5 */
	table_opcode_mem16_32[3]         = BITMASK32(0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  /* 6 */
					             0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0); /* 7 */
	table_opcode_mem16_32[4]         = BITMASK32(0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  /* 8 */
					             0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0); /* 9 */
	table_opcode_mem16_32[5]         = BITMASK32(1,1,1,1,0,0,0,0, 0,0,0,0,0,0,0,0,  /* a */
					             0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0); /* b */
	table_opcode_mem16_32[6]         = BITMASK32(0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  /* c */
					             0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0); /* d */
	table_opcode_mem16_32[7]         = BITMASK32(0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  /* e */
					             0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0); /* f */

/* ----------------------------- 0x0f <opcode> ------------------------------ */

	uint32_t	table_0f_opcode_modrm[TABLESIZE];
						  /* 0 1 2 3 4 5 6 7  8 9 a b c d e f */
	table_0f_opcode_modrm[0]         = BITMASK32(1,1,1,1,0,0,0,0, 0,0,0,0,0,0,0,0,  /* 0 */
						     1,1,1,1,1,1,1,1, 1,0,0,0,0,0,0,1); /* 1 */
	table_0f_opcode_modrm[1]         = BITMASK32(1,1,1,1,0,0,0,0, 1,1,1,1,1,1,1,1,  /* 2 */
						     0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0); /* 3 */
	table_0f_opcode_modrm[2]         = BITMASK32(1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,  /* 4 */
						     1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1); /* 5 */
	table_0f_opcode_modrm[3]         = BITMASK32(1,1,1,1,1,1,1,1, 1,1,1,1,0,0,1,1,  /* 6 */
						     1,1,1,1,1,1,1,0, 1,1,0,0,0,0,1,1); /* 7 */
	table_0f_opcode_modrm[4]         = BITMASK32(0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  /* 8 */
						     1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1); /* 9 */
	table_0f_opcode_modrm[5]         = BITMASK32(0,0,0,1,1,1,0,0, 0,0,0,1,1,1,1,1,  /* a */
						     1,1,1,1,1,1,1,1, 0,1,1,1,1,1,1,1); /* b */
	table_0f_opcode_modrm[6]         = BITMASK32(1,1,1,1,1,1,1,1, 0,0,0,0,0,0,0,0,  /* c */
						     0,1,1,1,1,1,0,1, 1,1,1,1,1,1,1,1); /* d */
	table_0f_opcode_modrm[7]         = BITMASK32(1,1,1,1,1,1,0,1, 1,1,1,1,1,1,1,1,  /* e */
						     0,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,0); /* f */
	uint32_t	table_0f_opcode_imm8[TABLESIZE];
					          /* 0 1 2 3 4 5 6 7  8 9 a b c d e f */
	table_0f_opcode_imm8[0]          = BITMASK32(0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  /* 0 */
					             0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0); /* 1 */
	table_0f_opcode_imm8[1]          = BITMASK32(0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  /* 2 */
					             0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0); /* 3 */
	table_0f_opcode_imm8[2]          = BITMASK32(0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  /* 4 */
					             0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0); /* 5 */
	table_0f_opcode_imm8[3]          = BITMASK32(0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  /* 6 */
					             0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0); /* 7 */
	table_0f_opcode_imm8[4]          = BITMASK32(0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  /* 8 */
					             0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0); /* 9 */
	table_0f_opcode_imm8[5]          = BITMASK32(0,0,0,0,1,0,0,0, 0,0,0,0,1,0,0,0,  /* a */
					             0,0,0,0,0,0,0,0, 0,0,1,0,0,0,0,0); /* b */
	table_0f_opcode_imm8[6]          = BITMASK32(0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  /* c */
					             0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0); /* d */
	table_0f_opcode_imm8[7]          = BITMASK32(0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  /* e */
					             0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0); /* f */
	uint32_t	table_0f_opcode_imm16_32[TABLESIZE];
					          /* 0 1 2 3 4 5 6 7  8 9 a b c d e f */
	table_0f_opcode_imm16_32[0]      = BITMASK32(0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  /* 0 */
						     0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0); /* 1 */
	table_0f_opcode_imm16_32[1]      = BITMASK32(0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  /* 2 */
						     0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0); /* 3 */
	table_0f_opcode_imm16_32[2]      = BITMASK32(0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  /* 4 */
						     0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0); /* 5 */
	table_0f_opcode_imm16_32[3]      = BITMASK32(0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  /* 6 */
						     0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0); /* 7 */
	table_0f_opcode_imm16_32[4]      = BITMASK32(1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,  /* 8 */
						     0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0); /* 9 */
	table_0f_opcode_imm16_32[5]      = BITMASK32(0,0,0,0,1,0,0,0, 0,0,0,0,1,0,0,0,  /* a */
						     0,0,0,0,0,0,0,0, 0,0,1,0,0,0,0,0); /* b */
	table_0f_opcode_imm16_32[6]      = BITMASK32(0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  /* c */
						     0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0); /* d */
	table_0f_opcode_imm16_32[7]      = BITMASK32(0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  /* e */
						     0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0); /* f */

	size_t		defmem   = DWORD;       /* memory size defined  */
	size_t		defdata  = DWORD;       /* operand size defined */
	size_t		memsize  = 0;           /* current memory size  */
	size_t		datasize = 0;           /* current data size    */
	int8_t		prefix   = 0;           /* prefix(es)           */
	int8_t		flags    = 0;           /* flag(s)              */
	bool		rex      = false;       /* has REX prefix       */

	uint8_t		*p = (uint8_t*)code;
	uint8_t		opcode;                 /* current opcode       */

	/* set <codelen> to INSTRUCTION_MAXLEN if it exceeds it */
	if (codelen > INSTRUCTION_MAXLEN) codelen = INSTRUCTION_MAXLEN;

next_opcode:
	if (!codelen--) return 0; /* error if instruction is too long */
	opcode = *p++;

	/* NULL byte prefixes            */
	if ((opcode == 0x26 || opcode == 0x2e
	||   opcode == 0x36 || opcode == 0x3e
	/* FS/GS segment override prefix */
	||   opcode == 0x64 || opcode == 0x65
	/* REX without W(idth) field     */
	||  (opcode >= 0x40 && opcode <= 0x47)
	/* assert LOCK# signal prefix    */
	||   opcode == 0xf0)
	&&  !(prefix))
		{goto next_opcode;}

	/* mandatory prefix for two-byte opcode              */
	else if (opcode == 0x0f) {prefix |= OP_PREFIX_0F; goto next_opcode;}
	/* address size overwrite: set memory size to 32-bit */
	else if (opcode == 0x67) {defmem  = WORD; goto next_opcode;}
	/* operand size overwrite: set operand size to 16-bit */
	else if (opcode == 0x66) {defdata = WORD; goto next_opcode;}
	/* operand size overwrite: REX.W 64-bit operand size */
	else if (opcode >= 0x48 && opcode <= 0x4f)
		{rex = true; goto next_opcode;}

	/* select a table according to mapping */
	if (prefix == MAP_0F)  /* 0x0f <opcode> */
	{
		if (CHECK_TABLE(table_0f_opcode_modrm, opcode))
			{flags |= MODRM;}
		if (CHECK_TABLE(table_0f_opcode_imm8, opcode))
			{datasize += BYTE;}
		if (CHECK_TABLE(table_0f_opcode_imm16_32, opcode))
			{datasize += defdata;}
	}
	else                   /* <opcode> */
	{
		/* exception: 64-bit immediates */
		if (CHECK_TABLE(table_opcode_imm64, opcode) && (rex == true))
			{defdata = QWORD;}
		/* possess MODRM byte          */
		if (CHECK_TABLE(table_opcode_modrm, opcode))
			{flags |= MODRM;}
		/* direct memory offset        */
		if (CHECK_TABLE(table_opcode_mem16_32, opcode))
			{memsize += defmem;}
		/* immediate values            */
		if (CHECK_TABLE(table_opcode_imm8, opcode))
			{datasize += BYTE;}
		if (CHECK_TABLE(table_opcode_imm16_32, opcode))
			{datasize += defdata;}
		/* exception: <test>            */
		if      (opcode == 0xf6) {flags |= TEST_F6;}
		else if (opcode == 0xf7) {flags |= TEST_F7;}
	}

	/* end if has no modrm byte */
	if (!(flags & MODRM)) {goto end;}

	/* error if instruction is too long */
	if (!codelen--) return 0;

	opcode = *p++;

	uint8_t		mod = (opcode & 0b11000000) >> 6;
	uint8_t		reg = (opcode & 0b00111000) >> 3;
	uint8_t		rm  = (opcode & 0b00000111);

	/* <test> exception */
	if ((flags & TEST) && (reg == 0b000 || reg == 0b001))
		{datasize += flags & TEST_F7 ? defdata : BYTE;}

	if      (mod == 0b11) {goto end;}          /* direct register addressing */
	else if (mod == 0b10) {memsize += defmem;} /* DWORD signed displacement  */
	else if (mod == 0b01) {memsize += BYTE;}   /* BYTE signed displacement   */

	/* 16-bit mode: (no SIB byte) and displacement */
	if (defmem == WORD && mod == 0b00 && rm == 0b110) {memsize += WORD;}
	/* 32-bit mode: (perhaps) SIB byte and displacement only */
	else
	{
		/* SIB with no displacement: get value of sib.base */
		if (rm == 0b100)
		{
			if (!codelen--) return 0; /* error if instruction is too long */
			rm = *p++ & 0b111;
		}
		/* displacement only */
		if (mod == 0b00 && rm == 0b101) {memsize += DWORD;}
	}
end:
	if (datasize + memsize > codelen) return 0; /* error if instruction is too long */

	p += datasize + memsize;
	return (void*)p - (void*)code;
}
