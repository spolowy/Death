
#include "disasm_utils.h"
#include "bytes.h"

/* opcode flags  */
# define MODRM		(1 << 0)            /* MODRM byte       */
# define TEST_F6	(1 << 1)            /* <test> exception */
# define TEST_F7	(1 << 2)            /* <test> exception */
# define TEST		(TEST_F6 | TEST_F7)

/*
** Disassemble an instruction pointed by <code> for a maximum
** length of <codelen>.
** It HAVE to return a value between 1 and 15 included.
** Returns 0 if failed.
*/
uint8_t		disasm_length(const void *code, size_t codelen)
{
	uint32_t	table_opcode_modrm_ext[TABLESIZE];
					          /* 0 1 2 3 4 5 6 7  8 9 a b c d e f */
	table_opcode_modrm_ext[0]        = BITMASK32(0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  /* 0 */
					             0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0); /* 1 */
	table_opcode_modrm_ext[1]        = BITMASK32(0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  /* 2 */
					             0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0); /* 3 */
	table_opcode_modrm_ext[2]        = BITMASK32(0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  /* 4 */
					             0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0); /* 5 */
	table_opcode_modrm_ext[3]        = BITMASK32(0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  /* 6 */
					             0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0); /* 7 */
	table_opcode_modrm_ext[4]        = BITMASK32(1,1,1,1,0,0,0,0, 0,0,0,0,0,0,0,1,  /* 8 */
					             0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0); /* 9 */
	table_opcode_modrm_ext[5]        = BITMASK32(0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  /* a */
					             0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0); /* b */
	table_opcode_modrm_ext[6]        = BITMASK32(1,1,0,0,0,0,1,1, 0,0,0,0,0,0,0,0,  /* c */
					             1,1,1,1,0,0,0,0, 1,1,1,1,1,1,1,1); /* d */
	table_opcode_modrm_ext[7]        = BITMASK32(0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  /* e */
					             0,0,0,0,0,0,1,1, 0,0,0,0,0,0,1,1); /* f */
	uint32_t	table_opcode_modrm_noext[TABLESIZE];
					          /* 0 1 2 3 4 5 6 7  8 9 a b c d e f */
	table_opcode_modrm_noext[0]      = BITMASK32(1,1,1,1,0,0,0,0, 1,1,1,1,0,0,0,0,  /* 0 */
						     1,1,1,1,0,0,0,0, 1,1,1,1,0,0,0,0); /* 1 */
	table_opcode_modrm_noext[1]      = BITMASK32(1,1,1,1,0,0,0,0, 1,1,1,1,0,0,0,0,  /* 2 */
						     1,1,1,1,0,0,0,0, 1,1,1,1,0,0,0,0); /* 3 */
	table_opcode_modrm_noext[2]      = BITMASK32(0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  /* 4 */
						     0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0); /* 5 */
	table_opcode_modrm_noext[3]      = BITMASK32(0,0,1,1,0,0,0,0, 0,1,0,1,0,0,0,0,  /* 6 */
						     0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0); /* 7 */
	table_opcode_modrm_noext[4]      = BITMASK32(0,0,0,0,1,1,1,1, 1,1,1,1,1,1,1,0,  /* 8 */
						     0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0); /* 9 */
	table_opcode_modrm_noext[5]      = BITMASK32(0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  /* a */
						     0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0); /* b */
	table_opcode_modrm_noext[6]      = BITMASK32(0,0,0,0,1,1,0,0, 0,0,0,0,0,0,0,0,  /* c */
						     0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0); /* d */
	table_opcode_modrm_noext[7]      = BITMASK32(0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  /* e */
						     0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0); /* f */
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
	uint32_t	table_opcode_imm16[TABLESIZE];
				                  /* 0 1 2 3 4 5 6 7  8 9 a b c d e f */
	table_opcode_imm16[0]            = BITMASK32(0,0,0,0,0,1,0,0, 0,0,0,0,0,1,0,0,  /* 0 */
					             0,0,0,0,0,1,0,0, 0,0,0,0,0,1,0,0); /* 1 */
	table_opcode_imm16[1]            = BITMASK32(0,0,0,0,0,1,0,0, 0,0,0,0,0,1,0,0,  /* 2 */
					             0,0,0,0,0,1,0,0, 0,0,0,0,0,1,0,0); /* 3 */
	table_opcode_imm16[2]            = BITMASK32(0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  /* 4 */
					             0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0); /* 5 */
	table_opcode_imm16[3]            = BITMASK32(0,0,0,0,0,0,0,0, 1,1,0,0,0,0,0,0,  /* 6 */
					             0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0); /* 7 */
	table_opcode_imm16[4]            = BITMASK32(0,1,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  /* 8 */
					             0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0); /* 9 */
	table_opcode_imm16[5]            = BITMASK32(0,0,0,0,0,0,0,0, 0,1,0,0,0,0,0,0,  /* a */
					             0,0,0,0,0,0,0,0, 1,1,1,1,1,1,1,1); /* b */
	table_opcode_imm16[6]            = BITMASK32(0,0,1,0,0,0,0,1, 1,0,1,0,0,0,0,0,  /* c */
					             0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0); /* d */
	table_opcode_imm16[7]            = BITMASK32(0,0,0,0,0,0,0,0, 1,1,0,0,0,0,0,0,  /* e */
					             0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0); /* f */
	uint32_t	table_opcode_imm32[TABLESIZE];
				                  /* 0 1 2 3 4 5 6 7  8 9 a b c d e f */
	table_opcode_imm32[0]            = BITMASK32(0,0,0,0,0,1,0,0, 0,0,0,0,0,1,0,0,  /* 0 */
					             0,0,0,0,0,1,0,0, 0,0,0,0,0,1,0,0); /* 1 */
	table_opcode_imm32[1]            = BITMASK32(0,0,0,0,0,1,0,0, 0,0,0,0,0,1,0,0,  /* 2 */
					             0,0,0,0,0,1,0,0, 0,0,0,0,0,1,0,0); /* 3 */
	table_opcode_imm32[2]            = BITMASK32(0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  /* 4 */
					             0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0); /* 5 */
	table_opcode_imm32[3]            = BITMASK32(0,0,0,0,0,0,0,0, 1,1,0,0,0,0,0,0,  /* 6 */
					             0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0); /* 7 */
	table_opcode_imm32[4]            = BITMASK32(0,1,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  /* 8 */
					             0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0); /* 9 */
	table_opcode_imm32[5]            = BITMASK32(0,0,0,0,0,0,0,0, 0,1,0,0,0,0,0,0,  /* a */
					             0,0,0,0,0,0,0,0, 1,1,1,1,1,1,1,1); /* b */
	table_opcode_imm32[6]            = BITMASK32(0,0,0,0,0,0,0,1, 1,0,0,0,0,0,0,0,  /* c */
					             0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0); /* d */
	table_opcode_imm32[7]            = BITMASK32(0,0,0,0,0,0,0,0, 1,1,0,0,0,0,0,0,  /* e */
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
	uint32_t	table_0f_opcode_modrm_noext[TABLESIZE];
						  /* 0 1 2 3 4 5 6 7  8 9 a b c d e f */
	table_0f_opcode_modrm_noext[0]   = BITMASK32(0,0,1,1,0,0,0,0, 0,0,0,0,0,0,0,0,  /* 0 */
						     1,1,1,1,1,1,1,1, 0,0,0,0,0,0,0,0); /* 1 */
	table_0f_opcode_modrm_noext[1]   = BITMASK32(1,1,1,1,0,0,0,0, 1,1,1,1,1,1,1,1,  /* 2 */
						     0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0); /* 3 */
	table_0f_opcode_modrm_noext[2]   = BITMASK32(1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,  /* 4 */
						     1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1); /* 5 */
	table_0f_opcode_modrm_noext[3]   = BITMASK32(1,1,1,1,1,1,1,1, 1,1,1,1,0,0,1,1,  /* 6 */
						     1,0,0,0,1,1,1,0, 1,1,0,0,0,0,1,1); /* 7 */
	table_0f_opcode_modrm_noext[4]   = BITMASK32(0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  /* 8 */
						     0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0); /* 9 */
	table_0f_opcode_modrm_noext[5]   = BITMASK32(0,0,0,1,1,1,0,0, 0,0,0,1,1,1,0,1,  /* a */
						     1,1,1,1,1,1,1,1, 0,1,0,1,1,1,1,1); /* b */
	table_0f_opcode_modrm_noext[6]   = BITMASK32(1,1,1,1,1,1,1,0, 0,0,0,0,0,0,0,0,  /* c */
						     0,1,1,1,1,1,0,1, 1,1,1,1,1,1,1,1); /* d */
	table_0f_opcode_modrm_noext[7]   = BITMASK32(1,1,1,1,1,1,0,1, 1,1,1,1,1,1,1,1,  /* e */
						     0,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,0); /* f */
	uint32_t	table_0f_opcode_modrm_ext[TABLESIZE];
					          /* 0 1 2 3 4 5 6 7  8 9 a b c d e f */
	table_0f_opcode_modrm_ext[0]     = BITMASK32(1,1,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  /* 0 */
						     0,0,0,0,0,0,0,0, 1,0,0,0,0,0,0,1); /* 1 */
	table_0f_opcode_modrm_ext[1]     = BITMASK32(0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  /* 2 */
						     0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0); /* 3 */
	table_0f_opcode_modrm_ext[2]     = BITMASK32(0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  /* 4 */
						     0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0); /* 5 */
	table_0f_opcode_modrm_ext[3]     = BITMASK32(0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  /* 6 */
						     0,1,1,1,0,0,0,0, 0,0,0,0,0,0,0,0); /* 7 */
	table_0f_opcode_modrm_ext[4]     = BITMASK32(0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  /* 8 */
						     1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1); /* 9 */
	table_0f_opcode_modrm_ext[5]     = BITMASK32(0,0,0,0,0,0,0,0, 0,0,0,0,0,0,1,0,  /* a */
						     0,0,0,0,0,0,0,0, 0,0,1,0,0,0,0,0); /* b */
	table_0f_opcode_modrm_ext[6]     = BITMASK32(0,0,0,0,0,0,0,1, 0,0,0,0,0,0,0,0,  /* c */
						     0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0); /* d */
	table_0f_opcode_modrm_ext[7]     = BITMASK32(0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  /* e */
						     0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0); /* f */
	uint32_t	table_0f_opcode_single[TABLESIZE];
					          /* 0 1 2 3 4 5 6 7  8 9 a b c d e f */
	table_0f_opcode_single[0]        = BITMASK32(0,0,0,0,0,0,1,0, 1,1,1,1,0,0,0,0,  /* 0 */
					             0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0); /* 1 */
	table_0f_opcode_single[1]        = BITMASK32(0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  /* 2 */
					             0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0); /* 3 */
	table_0f_opcode_single[2]        = BITMASK32(0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  /* 4 */
					             0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0); /* 5 */
	table_0f_opcode_single[3]        = BITMASK32(0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  /* 6 */
					             0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0); /* 7 */
	table_0f_opcode_single[4]        = BITMASK32(0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  /* 8 */
					             0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0); /* 9 */
	table_0f_opcode_single[5]        = BITMASK32(1,1,1,0,0,0,0,0, 1,1,1,0,0,0,0,0,  /* a */
					             0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0); /* b */
	table_0f_opcode_single[6]        = BITMASK32(0,0,0,0,0,0,0,0, 1,1,1,1,1,1,1,1,  /* c */
					             0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0); /* d */
	table_0f_opcode_single[7]        = BITMASK32(0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  /* e */
					             0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0); /* f */
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
	uint32_t	table_9b_opcode_modrm_ext[TABLESIZE];
					          /* 0 1 2 3 4 5 6 7  8 9 a b c d e f */
	table_9b_opcode_modrm_ext[0]     = BITMASK32(0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  /* 0 */
						     0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0); /* 1 */
	table_9b_opcode_modrm_ext[1]     = BITMASK32(0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  /* 2 */
						     0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0); /* 3 */
	table_9b_opcode_modrm_ext[2]     = BITMASK32(0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  /* 4 */
						     0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0); /* 5 */
	table_9b_opcode_modrm_ext[3]     = BITMASK32(0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  /* 6 */
						     0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0); /* 7 */
	table_9b_opcode_modrm_ext[4]     = BITMASK32(0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  /* 8 */
						     0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0); /* 9 */
	table_9b_opcode_modrm_ext[5]     = BITMASK32(0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  /* a */
						     0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0); /* b */
	table_9b_opcode_modrm_ext[6]     = BITMASK32(0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  /* c */
						     0,0,0,0,0,0,0,0, 0,1,0,1,0,1,0,0); /* d */
	table_9b_opcode_modrm_ext[7]     = BITMASK32(0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  /* e */
						     0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0); /* f */
	uint32_t	table_f20f_opcode_modrm_noext[TABLESIZE];
						  /* 0 1 2 3 4 5 6 7  8 9 a b c d e f */
	table_f20f_opcode_modrm_noext[0] = BITMASK32(0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  /* 0 */
						     1,1,1,0,0,0,0,0, 0,0,0,0,0,0,0,0); /* 1 */
	table_f20f_opcode_modrm_noext[1] = BITMASK32(0,0,0,0,0,0,0,0, 0,0,1,0,1,1,0,0,  /* 2 */
						     0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0); /* 3 */
	table_f20f_opcode_modrm_noext[2] = BITMASK32(0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  /* 4 */
						     1,0,0,0,0,0,0,0, 1,1,1,0,1,1,1,1); /* 5 */
	table_f20f_opcode_modrm_noext[3] = BITMASK32(0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  /* 6 */
						     1,0,0,0,0,0,0,0, 0,0,0,0,1,1,0,0); /* 7 */
	table_f20f_opcode_modrm_noext[4] = BITMASK32(0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  /* 8 */
						     0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0); /* 9 */
	table_f20f_opcode_modrm_noext[5] = BITMASK32(0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  /* a */
						     0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0); /* b */
	table_f20f_opcode_modrm_noext[6] = BITMASK32(0,0,1,0,0,0,0,0, 0,0,0,0,0,0,0,0,  /* c */
						     1,0,0,0,0,0,1,0, 0,0,0,0,0,0,0,0); /* d */
	table_f20f_opcode_modrm_noext[7] = BITMASK32(0,0,0,0,0,0,1,0, 0,0,0,0,0,0,0,0,  /* e */
						     1,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0); /* f */
	uint32_t	table_f20f_opcode_imm8[TABLESIZE];
					          /* 0 1 2 3 4 5 6 7  8 9 a b c d e f */
	table_f20f_opcode_imm8[0]        = BITMASK32(0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  /* 0 */
					             0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0); /* 1 */
	table_f20f_opcode_imm8[1]        = BITMASK32(0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  /* 2 */
					             0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0); /* 3 */
	table_f20f_opcode_imm8[2]        = BITMASK32(0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  /* 4 */
					             0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0); /* 5 */
	table_f20f_opcode_imm8[3]        = BITMASK32(0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  /* 6 */
					             1,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0); /* 7 */
	table_f20f_opcode_imm8[4]        = BITMASK32(0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  /* 8 */
					             0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0); /* 9 */
	table_f20f_opcode_imm8[5]        = BITMASK32(0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  /* a */
					             0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0); /* b */
	table_f20f_opcode_imm8[6]        = BITMASK32(0,0,1,0,0,0,0,0, 0,0,0,0,0,0,0,0,  /* c */
					             0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0); /* d */
	table_f20f_opcode_imm8[7]        = BITMASK32(0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  /* e */
					             0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0); /* f */
	uint32_t	table_f30f_opcode_modrm_noext[TABLESIZE];
						  /* 0 1 2 3 4 5 6 7  8 9 a b c d e f */
	table_f30f_opcode_modrm_noext[0] = BITMASK32(0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  /* 0 */
						     1,1,1,0,0,0,1,0, 0,0,0,0,0,0,0,0); /* 1 */
	table_f30f_opcode_modrm_noext[1] = BITMASK32(0,0,0,0,0,0,0,0, 0,0,1,0,1,1,0,0,  /* 2 */
						     0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0); /* 3 */
	table_f30f_opcode_modrm_noext[2] = BITMASK32(0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  /* 4 */
						     0,1,1,1,0,0,0,0, 1,1,1,1,1,1,1,1); /* 5 */
	table_f30f_opcode_modrm_noext[3] = BITMASK32(0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,1,  /* 6 */
						     1,0,0,0,0,0,0,0, 0,0,0,0,0,0,1,1); /* 7 */
	table_f30f_opcode_modrm_noext[4] = BITMASK32(0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  /* 8 */
						     0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0); /* 9 */
	table_f30f_opcode_modrm_noext[5] = BITMASK32(0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  /* a */
						     0,0,0,0,0,0,0,0, 1,0,0,0,0,0,0,0); /* b */
	table_f30f_opcode_modrm_noext[6] = BITMASK32(0,0,1,0,0,0,0,0, 0,0,0,0,0,0,0,0,  /* c */
						     0,0,0,0,0,0,1,0, 0,0,0,0,0,0,0,0); /* d */
	table_f30f_opcode_modrm_noext[7] = BITMASK32(0,0,0,0,0,0,1,0, 0,0,0,0,0,0,0,0,  /* e */
						     0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0); /* f */
	uint32_t	table_f30f_opcode_modrm_ext[TABLESIZE];
						  /* 0 1 2 3 4 5 6 7  8 9 a b c d e f */
	table_f30f_opcode_modrm_ext[0]   = BITMASK32(0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  /* 0 */
						     0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0); /* 1 */
	table_f30f_opcode_modrm_ext[1]   = BITMASK32(0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  /* 2 */
						     0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0); /* 3 */
	table_f30f_opcode_modrm_ext[2]   = BITMASK32(0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  /* 4 */
						     0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0); /* 5 */
	table_f30f_opcode_modrm_ext[3]   = BITMASK32(0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  /* 6 */
						     0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0); /* 7 */
	table_f30f_opcode_modrm_ext[4]   = BITMASK32(0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  /* 8 */
						     0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0); /* 9 */
	table_f30f_opcode_modrm_ext[5]   = BITMASK32(0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  /* a */
						     0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0); /* b */
	table_f30f_opcode_modrm_ext[6]   = BITMASK32(0,0,0,0,0,0,0,1, 0,0,0,0,0,0,0,0,  /* c */
						     0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0); /* d */
	table_f30f_opcode_modrm_ext[7]   = BITMASK32(0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  /* e */
						     0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0); /* f */
	uint32_t	table_f30f_opcode_imm8[TABLESIZE];
					          /* 0 1 2 3 4 5 6 7  8 9 a b c d e f */
	table_f30f_opcode_imm8[0]        = BITMASK32(0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  /* 0 */
					             0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0); /* 1 */
	table_f30f_opcode_imm8[1]        = BITMASK32(0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  /* 2 */
					             0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0); /* 3 */
	table_f30f_opcode_imm8[2]        = BITMASK32(0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  /* 4 */
					             0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0); /* 5 */
	table_f30f_opcode_imm8[3]        = BITMASK32(0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  /* 6 */
					             1,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0); /* 7 */
	table_f30f_opcode_imm8[4]        = BITMASK32(0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  /* 8 */
					             0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0); /* 9 */
	table_f30f_opcode_imm8[5]        = BITMASK32(0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  /* a */
					             0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0); /* b */
	table_f30f_opcode_imm8[6]        = BITMASK32(0,0,1,0,0,0,0,0, 0,0,0,0,0,0,0,0,  /* c */
					             0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0); /* d */
	table_f30f_opcode_imm8[7]        = BITMASK32(0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  /* e */
					             0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0); /* f */
	uint32_t	table_660f_opcode_modrm_noext[TABLESIZE];
						  /* 0 1 2 3 4 5 6 7  8 9 a b c d e f */
	table_660f_opcode_modrm_noext[0] = BITMASK32(0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  /* 0 */
						     1,1,1,1,1,1,1,1, 0,0,0,0,0,0,0,0); /* 1 */
	table_660f_opcode_modrm_noext[1] = BITMASK32(0,0,0,0,0,0,0,0, 1,1,1,1,1,1,1,1,  /* 2 */
						     0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0); /* 3 */
	table_660f_opcode_modrm_noext[2] = BITMASK32(0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  /* 4 */
						     1,1,0,0,1,1,1,1, 1,1,1,1,1,1,1,1); /* 5 */
	table_660f_opcode_modrm_noext[3] = BITMASK32(1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,  /* 6 */
						     1,0,0,0,1,1,1,0, 0,0,0,0,1,1,1,1); /* 7 */
	table_660f_opcode_modrm_noext[4] = BITMASK32(0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  /* 8 */
						     0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0); /* 9 */
	table_660f_opcode_modrm_noext[5] = BITMASK32(0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  /* a */
						     0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0); /* b */
	table_660f_opcode_modrm_noext[6] = BITMASK32(0,0,1,0,1,1,1,0, 0,0,0,0,0,0,0,0,  /* c */
						     1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1); /* d */
	table_660f_opcode_modrm_noext[7] = BITMASK32(1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,  /* e */
						     0,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,0); /* f */
	uint32_t	table_660f_opcode_modrm_ext[TABLESIZE];
						  /* 0 1 2 3 4 5 6 7  8 9 a b c d e f */
	table_660f_opcode_modrm_ext[0]   = BITMASK32(0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  /* 0 */
						     0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0); /* 1 */
	table_660f_opcode_modrm_ext[1]   = BITMASK32(0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  /* 2 */
						     0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0); /* 3 */
	table_660f_opcode_modrm_ext[2]   = BITMASK32(0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  /* 4 */
						     0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0); /* 5 */
	table_660f_opcode_modrm_ext[3]   = BITMASK32(0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  /* 6 */
						     0,1,1,1,0,0,0,0, 0,0,0,0,0,0,0,0); /* 7 */
	table_660f_opcode_modrm_ext[4]   = BITMASK32(0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  /* 8 */
						     0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0); /* 9 */
	table_660f_opcode_modrm_ext[5]   = BITMASK32(0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  /* a */
						     0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0); /* b */
	table_660f_opcode_modrm_ext[6]   = BITMASK32(0,0,0,0,0,0,0,1, 0,0,0,0,0,0,0,0,  /* c */
						     0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0); /* d */
	table_660f_opcode_modrm_ext[7]   = BITMASK32(0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  /* e */
						     0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0); /* f */
	uint32_t	table_660f_opcode_imm8[TABLESIZE];
					          /* 0 1 2 3 4 5 6 7  8 9 a b c d e f */
	table_660f_opcode_imm8[0]        = BITMASK32(0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  /* 0 */
					             0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0); /* 1 */
	table_660f_opcode_imm8[1]        = BITMASK32(0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  /* 2 */
					             0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0); /* 3 */
	table_660f_opcode_imm8[2]        = BITMASK32(0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  /* 4 */
					             0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0); /* 5 */
	table_660f_opcode_imm8[3]        = BITMASK32(0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  /* 6 */
					             1,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0); /* 7 */
	table_660f_opcode_imm8[4]        = BITMASK32(0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  /* 8 */
					             0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0); /* 9 */
	table_660f_opcode_imm8[5]        = BITMASK32(0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  /* a */
					             0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0); /* b */
	table_660f_opcode_imm8[6]        = BITMASK32(0,0,1,0,1,1,1,0, 0,0,0,0,0,0,0,0,  /* c */
					             0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0); /* d */
	table_660f_opcode_imm8[7]        = BITMASK32(0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  /* e */
					             0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0); /* f */

	size_t		defmem   = DWORD;    /* memory size defined  */
	size_t		defdata  = DWORD;    /* operand size defined */
	size_t		memsize  = 0;        /* current memory size  */
	size_t		datasize = 0;        /* current data size    */
	int8_t		prefix   = 0;        /* prefix(es)           */
	int8_t		flags    = 0;        /* flag(s)              */
	bool		rex      = false;    /* rex prefix           */

	uint8_t		*p = (uint8_t*)code;
	uint8_t		opcode;              /* current opcode */

	/* set <codelen> to INSTRUCTION_MAXLEN if it exceeds it */
	if (codelen > INSTRUCTION_MAXLEN) codelen = INSTRUCTION_MAXLEN;

	next_opcode:
	if (!codelen--) return 0; /* error if instruction is too long */
	opcode = *p++;

	/* skip unused legacy / REX prefixes for this disassembler */
	if ((opcode == 0x26 || opcode == 0x2e
	||   opcode == 0x36 || opcode == 0x3e
	||   opcode == 0x64 || opcode == 0x65
	||   opcode == 0x40 || opcode == 0x41 || opcode == 0x42 || opcode == 0x43
	||   opcode == 0x44 || opcode == 0x45 || opcode == 0x46 || opcode == 0x47
	||   opcode == 0xf0)
	&&  !(prefix))
		{goto next_opcode;}
	/* operand size overwrite */
	else if (opcode == 0x66) {defdata = WORD; prefix |= OP_PREFIX_66; goto next_opcode;} /* set operand size to 16-bit; get mandatory prefix */
	/* address size overwrite */
	else if (opcode == 0x67) {defmem = DWORD; goto next_opcode;}                         /* set memory size to 32-bit */
	/* mandatory prefixes     */
	else if (opcode == 0x0f) {prefix |= OP_PREFIX_0F; goto next_opcode;}                 /* get mandatory prefix */
	else if (opcode == 0xf2) {prefix |= OP_PREFIX_F2; goto next_opcode;}                 /* get mandatory prefix */
	else if (opcode == 0xf3) {prefix |= OP_PREFIX_F3; goto next_opcode;}                 /* get mandatory prefix */
	else if (opcode == 0x9b) {prefix |= OP_PREFIX_9B; goto next_opcode;}                 /* get mandatory prefix */
	/* REX.W operand size overwrite */
	else if ((opcode == 0x48 || opcode == 0x49
	     ||   opcode == 0x4a || opcode == 0x4b
	     ||   opcode == 0x4c || opcode == 0x4d
	     ||   opcode == 0x4e || opcode == 0x4f)
	     && !(prefix))
		{rex = true; goto next_opcode;} /* rex 64-bit operand modifier */

	/* choose specific tables for different mappings */
	if (prefix == MAP_9B) /* 0x9b <opcode> */
	{
		if (CHECK_TABLE(table_9b_opcode_modrm_ext, opcode))
			flags |= MODRM;
	}
	else if (prefix == MAP_660F)  /* 0x66 0x0f <opcode> */
	{
		if (CHECK_TABLE(table_660f_opcode_modrm_noext, opcode)
		||  CHECK_TABLE(table_660f_opcode_modrm_ext, opcode))
			flags |= MODRM;
		if (CHECK_TABLE(table_660f_opcode_imm8, opcode))
			datasize += defdata;
	}
	else if (prefix == MAP_F20F)  /* 0xf2 0x0f <opcode> */
	{
		if (CHECK_TABLE(table_f20f_opcode_modrm_noext, opcode))
			flags |= MODRM;
		if (CHECK_TABLE(table_f20f_opcode_imm8, opcode))
			datasize += defdata;
	}
	else if (prefix == MAP_F30F) /* 0xf3 0x0f <opcode> */
	{
		if (CHECK_TABLE(table_f30f_opcode_modrm_noext, opcode)
		||  CHECK_TABLE(table_f30f_opcode_modrm_ext, opcode))
			flags |= MODRM;
		if (CHECK_TABLE(table_f30f_opcode_imm8, opcode))
			datasize += defdata;
	}
	else if (prefix == MAP_0F)  /* 0x0f <opcode>       */
	{
		if (CHECK_TABLE(table_0f_opcode_modrm_noext, opcode)
		||  CHECK_TABLE(table_0f_opcode_modrm_ext, opcode))
			flags |= MODRM;
		if (CHECK_TABLE(table_0f_opcode_imm8, opcode))
			datasize += defdata;
		if (CHECK_TABLE(table_0f_opcode_imm16_32, opcode))
			datasize += defdata;
	}
	else                        /* <opcode>            */
	{
		if (CHECK_TABLE(table_opcode_imm64, opcode) && (rex == true))
		{
			defdata = QWORD; /* exception for 64-bit immediates */
		}
		if (CHECK_TABLE(table_opcode_modrm_noext, opcode)
		||  CHECK_TABLE(table_opcode_modrm_ext, opcode))
			flags |= MODRM;
		if (opcode == 0xf6) /* exception for <test> which has the same opcode as other instructions with different length */
			flags |= TEST_F6;
		else if (opcode == 0xf7)
			flags |= TEST_F7;
		if (CHECK_TABLE(table_opcode_imm8, opcode))
			datasize += BYTE;    /* 1 byte immediate          */
		if (CHECK_TABLE(table_opcode_imm16, opcode)
		||  CHECK_TABLE(table_opcode_imm32, opcode))
			datasize += defdata; /* 2 or 4 byte immediate     */
		if (CHECK_TABLE(table_opcode_mem16_32, opcode))
			memsize  += defmem;  /* direct memory offset case */
	}

	/* MODRM byte management */
	if (flags & MODRM)
	{
		if (!codelen--) return 0; /* error if instruction is too long */
		opcode = *p++;
		uint8_t		mod = (opcode & 0b11000000) >> 6;
		uint8_t		reg = (opcode & 0b00111000) >> 3;
		uint8_t		rm  = opcode & 0b00000111;

		if ((flags & TEST) && (reg == 0b000 || reg == 0b001)) /* <test> exception */
			{datasize += flags & TEST_F7 ? defdata : BYTE;}
		if (mod != 0b11) /* addressing mode is not direct register addressing */
		{
			if      (mod == 0b01) {memsize += BYTE;}   /* one byte signed displacement  */
			else if (mod == 0b10) {memsize += defmem;} /* four byte signed displacement */
			if (defmem == WORD) /* 16-bit mode (no SIB byte) */
			{
				if (mod == 0b00 && rm == 0b110) {memsize += WORD;} /* 16-bit displacement */
			}
			else /* SIB byte may be present */
			{
				if (rm == 0b100) /* SIB addressing and get base register */
				{
					if (!codelen--) return 0; /* error if instruction is too long */
					rm = *p++ & 0b111;
				}
				if (mod == 0b00 && rm == 0b101) {memsize += DWORD;} /* 32-bit displacement mode */
			}
		}
	}

	if ((datasize + memsize) > codelen) return 0; /* error if instruction is too long */

	p += datasize + memsize;
	return (void*)p - (void*)code;
}
