#include <stdbool.h>
#include "accessors.h"
#include "compiler_utils.h"
#include "disasm_utils.h"
#include "utils.h"
#include "errors.h"

#define REX_R(rex)	(!!(rex & 0b100)) /* MODRM (R)eg field                         */
#define REX_X(rex)	(!!(rex & 0b010)) /* SIB inde(X) field                         */
#define REX_B(rex)	(!!(rex & 0b001)) /* MODRM r/m, SIB (B)ase or OPCODE reg field */

#define ENCODE_FLAG(reg, mode)	(1 << (reg + (mode << 3)))

/*
** Flags used to specify how the intruction should be interpreted
*/
#define MODRM		(1 << 0)  /* MODRM byte with register usage          */
#define EXTENSION	(1 << 1)  /* MODRM byte with opcode extension        */
#define EXT_SRC		(1 << 2)  /* extension assigned to source            */
#define EXT_DST		(1 << 3)  /* extension assigned to destination       */
#define KEEP_SRC	(1 << 4)  /* source register is also a destination   */
#define KEEP_DST	(1 << 5)  /* destination register is also a source   */
#define IMPLICIT_SRC	(1 << 6)  /* source register is implicit             */
#define IMPLICIT_DST	(1 << 7)  /* destination register is implicit        */
#define NONE_SRC	(1 << 8)  /* no final source reg/mem operand         */
#define MOVE_DST	(1 << 9)  /* move destination reg/mem to source      */
#define PROMOTE_SRC	(1 << 10) /* source register code Rx promotion       */
#define PROMOTE_DST	(1 << 11) /* destination register code Rx promotion  */
/*
** Opcode fields
*/
#define FLD_R 		(1 << 12) /* register code added to primary opcode   */
#define FLD_D		(1 << 13) /* direction of data operation             */

struct x64_set
{
	uint16_t	status;    /* directives          */
	uint32_t	dst_flags; /* destination operand */
	uint32_t	src_flags; /* source operand      */
};

struct dsm
{
	uint32_t	*src;      /* source              */
	uint32_t	*dst;      /* destination         */
	uint8_t		rex;       /* rex byte            */
	bool		direction; /* operation direction */
};

/* -------------------------------------------------------------------------- */

static struct x64_set	map_single(uint8_t *p, uint8_t opcode, size_t codelen)
{
	struct x64_set	i;
	struct x64_set	instructions[0xff+1];

	bzero(instructions, sizeof(instructions));

	instructions[0x00] = (struct x64_set){MODRM|FLD_D|KEEP_DST                                ,FLAGS     ,         0}; /* add    r/m8        r8                 */
	instructions[0x01] = (struct x64_set){MODRM|FLD_D|KEEP_DST                                ,FLAGS     ,         0}; /* add    r/m16/32/64 r16/32/64          */
	instructions[0x03] = (struct x64_set){MODRM|FLD_D|KEEP_DST                                ,FLAGS     ,         0}; /* add    r16/32/64   r/m16/32/64        */
	instructions[0x05] = (struct x64_set){IMPLICIT_DST|KEEP_DST                               ,FLAGS|RAX ,         0}; /* add    rAX         imm16/32           */
	instructions[0x09] = (struct x64_set){MODRM|FLD_D|KEEP_DST                                ,FLAGS     ,         0}; /* or     r/m16/32/64 r16/32/64          */
	instructions[0x0b] = (struct x64_set){MODRM|FLD_D|KEEP_DST                                ,FLAGS     ,         0}; /* or     r16/32/64   r/m16/32/64        */
	instructions[0x21] = (struct x64_set){MODRM|FLD_D|KEEP_DST                                ,FLAGS     ,         0}; /* and    r16/32/64   r/m16/32/64        */
	instructions[0x23] = (struct x64_set){MODRM|FLD_D|KEEP_DST                                ,FLAGS     ,         0}; /* and    r16/32/64   r/m16/32/64        */
	instructions[0x24] = (struct x64_set){IMPLICIT_DST|KEEP_DST                               ,FLAGS|RAX ,         0}; /* and    al          imm8               */
	instructions[0x25] = (struct x64_set){IMPLICIT_DST|KEEP_DST                               ,FLAGS|RAX ,         0}; /* and    rAX         imm16/32           */
	instructions[0x29] = (struct x64_set){MODRM|FLD_D|KEEP_DST                                ,FLAGS     ,         0}; /* sub    r/m16/32/64 r16/32/64          */
	instructions[0x2b] = (struct x64_set){MODRM|FLD_D|KEEP_DST                                ,FLAGS     ,         0}; /* sub    r16/32/64   r/m16/32/64        */
	instructions[0x31] = (struct x64_set){MODRM|FLD_D|KEEP_DST                                ,FLAGS     ,         0}; /* xor    r/m16/32/64 r16/32/64          */
	instructions[0x33] = (struct x64_set){MODRM|FLD_D|KEEP_DST                                ,FLAGS     ,         0}; /* xor    r16/32/64   r/m16/32/64        */
	instructions[0x34] = (struct x64_set){IMPLICIT_DST|KEEP_DST                               ,FLAGS|RAX ,         0}; /* xor    al          imm8               */
	instructions[0x39] = (struct x64_set){MODRM|FLD_D|KEEP_DST|MOVE_DST|KEEP_DST              ,FLAGS     ,         0}; /* cmp    r/m16/32/64 r16/32/64          */
	instructions[0x3b] = (struct x64_set){MODRM|FLD_D|KEEP_DST|MOVE_DST|KEEP_DST              ,FLAGS     ,         0}; /* cmp    r16/32/64   r/m16/32/64        */
	instructions[0x3d] = (struct x64_set){IMPLICIT_SRC|KEEP_DST|MOVE_DST                      ,FLAGS     ,RAX       }; /* cmp    rAX         imm16/32           */
	instructions[0x50] = (struct x64_set){FLD_R|IMPLICIT_DST|IMPLICIT_SRC|KEEP_DST|PROMOTE_SRC,MEMORY|RSP,RAX|RSP   }; /* push   rAX/r8                         */
	instructions[0x51] = (struct x64_set){FLD_R|IMPLICIT_DST|IMPLICIT_SRC|KEEP_DST|PROMOTE_SRC,MEMORY|RSP,RCX|RSP   }; /* push   rCX/r9                         */
	instructions[0x52] = (struct x64_set){FLD_R|IMPLICIT_DST|IMPLICIT_SRC|KEEP_DST|PROMOTE_SRC,MEMORY|RSP,RDX|RSP   }; /* push   rDX/r10                        */
	instructions[0x53] = (struct x64_set){FLD_R|IMPLICIT_DST|IMPLICIT_SRC|KEEP_DST|PROMOTE_SRC,MEMORY|RSP,RBX|RSP   }; /* push   rBX/r11                        */
	instructions[0x54] = (struct x64_set){FLD_R|IMPLICIT_DST|IMPLICIT_SRC|KEEP_DST|PROMOTE_SRC,MEMORY|RSP,RSP|RSP   }; /* push   rSP/r12                        */
	instructions[0x55] = (struct x64_set){FLD_R|IMPLICIT_DST|IMPLICIT_SRC|KEEP_DST|PROMOTE_SRC,MEMORY|RSP,RBP|RSP   }; /* push   rBP/r13                        */
	instructions[0x56] = (struct x64_set){FLD_R|IMPLICIT_DST|IMPLICIT_SRC|KEEP_DST|PROMOTE_SRC,MEMORY|RSP,RSI|RSP   }; /* push   rSI/r14                        */
	instructions[0x57] = (struct x64_set){FLD_R|IMPLICIT_DST|IMPLICIT_SRC|KEEP_DST|PROMOTE_SRC,MEMORY|RSP,RDI|RSP   }; /* push   rDI/r15                        */
	instructions[0x58] = (struct x64_set){FLD_R|IMPLICIT_DST|IMPLICIT_SRC|KEEP_SRC|PROMOTE_DST,RAX|RSP   ,MEMORY|RSP}; /* pop    rAX/r8                         */
	instructions[0x59] = (struct x64_set){FLD_R|IMPLICIT_DST|IMPLICIT_SRC|KEEP_SRC|PROMOTE_DST,RCX|RSP   ,MEMORY|RSP}; /* pop    rCX/r9                         */
	instructions[0x5a] = (struct x64_set){FLD_R|IMPLICIT_DST|IMPLICIT_SRC|KEEP_SRC|PROMOTE_DST,RDX|RSP   ,MEMORY|RSP}; /* pop    rDX/r10                        */
	instructions[0x5b] = (struct x64_set){FLD_R|IMPLICIT_DST|IMPLICIT_SRC|KEEP_SRC|PROMOTE_DST,RBX|RSP   ,MEMORY|RSP}; /* pop    rBX/r11                        */
	instructions[0x5c] = (struct x64_set){FLD_R|IMPLICIT_DST|IMPLICIT_SRC|KEEP_SRC|PROMOTE_DST,RSP|RSP   ,MEMORY|RSP}; /* pop    rSP/r12                        */
	instructions[0x5d] = (struct x64_set){FLD_R|IMPLICIT_DST|IMPLICIT_SRC|KEEP_SRC|PROMOTE_DST,RBP|RSP   ,MEMORY|RSP}; /* pop    rBP/r13                        */
	instructions[0x5e] = (struct x64_set){FLD_R|IMPLICIT_DST|IMPLICIT_SRC|KEEP_SRC|PROMOTE_DST,RSI|RSP   ,MEMORY|RSP}; /* pop    rSI/r14                        */
	instructions[0x5f] = (struct x64_set){FLD_R|IMPLICIT_DST|IMPLICIT_SRC|KEEP_SRC|PROMOTE_DST,RDI|RSP   ,MEMORY|RSP}; /* pop    rDI/r15                        */
	instructions[0x63] = (struct x64_set){MODRM|FLD_D                                         ,         0,         0}; /* movsxd r32/64      r/m32              */
	instructions[0x80] = (struct x64_set){EXTENSION                                           ,         0,         0}; /*                                       */
	instructions[0x81] = (struct x64_set){EXTENSION                                           ,         0,         0}; /*                                       */
	instructions[0x83] = (struct x64_set){EXTENSION                                           ,         0,         0}; /*                                       */
	instructions[0x85] = (struct x64_set){MODRM|KEEP_SRC|MOVE_DST                             ,FLAGS     ,         0}; /* test   r/m16/32/64 reg16/32/64        */
	instructions[0x87] = (struct x64_set){MODRM|KEEP_SRC|KEEP_DST                             ,         0,         0}; /* xchg   reg16/32/64 r/m16/32/64        */
	instructions[0x88] = (struct x64_set){MODRM|FLD_D                                         ,         0,         0}; /* mov    r/m8        r8                 */
	instructions[0x89] = (struct x64_set){MODRM|FLD_D                                         ,         0,         0}; /* mov    r/m16/32/64 r16/32/64          */
	instructions[0x8a] = (struct x64_set){MODRM|FLD_D                                         ,         0,         0}; /* mov    r8          r/m8               */
	instructions[0x8b] = (struct x64_set){MODRM|FLD_D                                         ,         0,         0}; /* mov    r16/32/64   r/m16/32/64        */
	instructions[0x8d] = (struct x64_set){MODRM                                               ,         0,MEMORY    }; /* lea    r16/32/64   m                  */
	instructions[0x99] = (struct x64_set){IMPLICIT_DST|IMPLICIT_SRC                           ,RDX       ,RAX       }; /* cqo                                   */
	instructions[0xa8] = (struct x64_set){IMPLICIT_SRC|MOVE_DST                               ,FLAGS     ,RAX       }; /* test   al          imm8               */
	instructions[0xb0] = (struct x64_set){FLD_R|IMPLICIT_DST|PROMOTE_DST                      ,RAX       ,         0}; /* mov    rAX         imm8               */
	instructions[0xb8] = (struct x64_set){FLD_R|IMPLICIT_DST|PROMOTE_DST                      ,RAX       ,         0}; /* mov    reAX        imm16/32/64        */
	instructions[0xb9] = (struct x64_set){FLD_R|IMPLICIT_DST|PROMOTE_DST                      ,RCX       ,         0}; /* mov    reCX        imm16/32/64        */
	instructions[0xba] = (struct x64_set){FLD_R|IMPLICIT_DST|PROMOTE_DST                      ,RDX       ,         0}; /* mov    reDX        imm16/32/64        */
	instructions[0xbb] = (struct x64_set){FLD_R|IMPLICIT_DST|PROMOTE_DST                      ,RBX       ,         0}; /* mov    reBX        imm16/32/64        */
	instructions[0xbc] = (struct x64_set){FLD_R|IMPLICIT_DST|PROMOTE_DST                      ,RSP       ,         0}; /* mov    reSP        imm16/32/64        */
	instructions[0xbd] = (struct x64_set){FLD_R|IMPLICIT_DST|PROMOTE_DST                      ,RBP       ,         0}; /* mov    reBP        imm16/32/64        */
	instructions[0xbe] = (struct x64_set){FLD_R|IMPLICIT_DST|PROMOTE_DST                      ,RSI       ,         0}; /* mov    RSI         imm16/32/64        */
	instructions[0xbf] = (struct x64_set){FLD_R|IMPLICIT_DST|PROMOTE_DST                      ,RDI       ,         0}; /* mov    RDI         imm16/32/64        */
	instructions[0xc1] = (struct x64_set){EXTENSION                                           ,         0,         0}; /*                                       */
	instructions[0xc3] = (struct x64_set){IMPLICIT_DST|IMPLICIT_SRC|KEEP_SRC                  ,RIP       ,MEMORY|RSP}; /* ret                                   */
	instructions[0xc6] = (struct x64_set){EXTENSION                                           ,         0,         0}; /*                                       */
	instructions[0xc7] = (struct x64_set){EXTENSION                                           ,         0,         0}; /*                                       */
	instructions[0xd3] = (struct x64_set){EXTENSION                                           ,         0,         0}; /*                                       */
	instructions[0xf6] = (struct x64_set){EXTENSION                                           ,         0,         0}; /*                                       */
	instructions[0xf7] = (struct x64_set){EXTENSION                                           ,         0,         0}; /*                                       */

	i = instructions[opcode];

	if (i.status & EXTENSION)
	{
		if (!codelen--) return (struct x64_set){0,0,0}; /* error if instruction is too long */
		uint8_t		modrm = *p;
		uint8_t		reg   = (modrm & 0b00111000) >> 3;

		struct x64_set	extensions[8];

		bzero(instructions, sizeof(instructions));

		if (opcode == 0x80)
		{
			extensions[0] = (struct x64_set){EXTENSION|KEEP_DST         ,FLAGS,    0}; /* add r/m8 imm8 */
			extensions[1] = (struct x64_set){EXTENSION|KEEP_DST         ,FLAGS,    0}; /* or  r/m8 imm8 */
			extensions[2] = (struct x64_set){EXTENSION|KEEP_DST         ,FLAGS,FLAGS}; /* adc r/m8 imm8 */
			extensions[3] = (struct x64_set){EXTENSION|KEEP_DST         ,FLAGS,FLAGS}; /* sbb r/m8 imm8 */
			extensions[4] = (struct x64_set){EXTENSION|KEEP_DST         ,FLAGS,    0}; /* and r/m8 imm8 */
			extensions[5] = (struct x64_set){EXTENSION|KEEP_DST         ,FLAGS,    0}; /* sub r/m8 imm8 */
			extensions[6] = (struct x64_set){EXTENSION|KEEP_DST         ,FLAGS,    0}; /* xor r/m8 imm8 */
			extensions[7] = (struct x64_set){EXTENSION|KEEP_DST|MOVE_DST,FLAGS,    0}; /* cmp r/m8 imm8 */
		}
		else if (opcode == 0x81)
		{
			extensions[0] = (struct x64_set){EXTENSION|KEEP_DST         ,FLAGS,    0}; /* add r/m16/32/64 imm16/32 */
			extensions[1] = (struct x64_set){EXTENSION|KEEP_DST         ,FLAGS,    0}; /* or  r/m16/32/64 imm16/32 */
			extensions[2] = (struct x64_set){EXTENSION|KEEP_DST         ,FLAGS,FLAGS}; /* adc r/m16/32/64 imm16/32 */
			extensions[3] = (struct x64_set){EXTENSION|KEEP_DST         ,FLAGS,FLAGS}; /* sbb r/m16/32/64 imm16/32 */
			extensions[4] = (struct x64_set){EXTENSION|KEEP_DST         ,FLAGS,    0}; /* and r/m16/32/64 imm16/32 */
			extensions[5] = (struct x64_set){EXTENSION|KEEP_DST         ,FLAGS,    0}; /* sub r/m16/32/64 imm16/32 */
			extensions[6] = (struct x64_set){EXTENSION|KEEP_DST         ,FLAGS,    0}; /* xor r/m16/32/64 imm16/32 */
			extensions[7] = (struct x64_set){EXTENSION|KEEP_DST|MOVE_DST,FLAGS,    0}; /* cmp r/m16/32/64 imm16/32 */
		}
		else if (opcode == 0x83)
		{
			extensions[0] = (struct x64_set){EXTENSION|KEEP_DST         ,FLAGS,    0}; /* add r/m16/32/64 imm8 */
			extensions[1] = (struct x64_set){EXTENSION|KEEP_DST         ,FLAGS,    0}; /* or  r/m16/32/64 imm8 */
			extensions[2] = (struct x64_set){EXTENSION|KEEP_DST         ,FLAGS,FLAGS}; /* adc r/m16/32/64 imm8 */
			extensions[3] = (struct x64_set){EXTENSION|KEEP_DST         ,FLAGS,FLAGS}; /* sbb r/m16/32/64 imm8 */
			extensions[4] = (struct x64_set){EXTENSION|KEEP_DST         ,FLAGS,    0}; /* and r/m16/32/64 imm8 */
			extensions[5] = (struct x64_set){EXTENSION|KEEP_DST         ,FLAGS,    0}; /* sub r/m16/32/64 imm8 */
			extensions[6] = (struct x64_set){EXTENSION|KEEP_DST         ,FLAGS,    0}; /* xor r/m16/32/64 imm8 */
			extensions[7] = (struct x64_set){EXTENSION|KEEP_DST|MOVE_DST|MOVE_DST,FLAGS,    0}; /* cmp r/m16/32/64 imm8 */
		}
		else if (opcode == 0xc1)
		{
			extensions[0] = (struct x64_set){EXTENSION|KEEP_DST,FLAGS,    0}; /* rol     r/m16/32/64 imm8 */
			extensions[1] = (struct x64_set){EXTENSION|KEEP_DST,FLAGS,    0}; /* ror     r/m16/32/64 imm8 */
			extensions[2] = (struct x64_set){EXTENSION|KEEP_DST,FLAGS,FLAGS}; /* rcl     r/m16/32/64 imm8 */
			extensions[3] = (struct x64_set){EXTENSION|KEEP_DST,FLAGS,FLAGS}; /* rcr     r/m16/32/64 imm8 */
			extensions[4] = (struct x64_set){EXTENSION|KEEP_DST,FLAGS,    0}; /* shl/sal r/m16/32/64 imm8 */
			extensions[5] = (struct x64_set){EXTENSION|KEEP_DST,FLAGS,    0}; /* shr     r/m16/32/64 imm8 */
			extensions[6] = (struct x64_set){EXTENSION|KEEP_DST,FLAGS,    0}; /* shl/sal r/m16/32/64 imm8 */
			extensions[7] = (struct x64_set){EXTENSION|KEEP_DST,FLAGS,    0}; /* sar     r/m16/32/64 imm8 */
		}
		else if (opcode == 0xc6)
		{
			extensions[0] = (struct x64_set){EXTENSION,0,0}; /* mov r/m8 imm8 */
			extensions[1] = (struct x64_set){        0,0,0}; /* undefined     */
			extensions[2] = (struct x64_set){        0,0,0}; /* undefined     */
			extensions[3] = (struct x64_set){        0,0,0}; /* undefined     */
			extensions[4] = (struct x64_set){        0,0,0}; /* undefined     */
			extensions[5] = (struct x64_set){        0,0,0}; /* undefined     */
			extensions[6] = (struct x64_set){        0,0,0}; /* undefined     */
			extensions[7] = (struct x64_set){        0,0,0}; /* undefined     */
		}
		else if (opcode == 0xc7)
		{
			extensions[0] = (struct x64_set){EXTENSION,0,0}; /* mov r/m16/32/64 imm16/32 */
			extensions[1] = (struct x64_set){        0,0,0}; /* undefined                */
			extensions[2] = (struct x64_set){        0,0,0}; /* undefined                */
			extensions[3] = (struct x64_set){        0,0,0}; /* undefined                */
			extensions[4] = (struct x64_set){        0,0,0}; /* undefined                */
			extensions[5] = (struct x64_set){        0,0,0}; /* undefined                */
			extensions[6] = (struct x64_set){        0,0,0}; /* undefined                */
			extensions[7] = (struct x64_set){        0,0,0}; /* undefined                */
		}
		else if (opcode == 0xd3)
		{
			extensions[0] = (struct x64_set){EXTENSION|IMPLICIT_SRC|KEEP_DST,FLAGS,RCX      }; /* rol     r/m16/32/64 cl */
			extensions[1] = (struct x64_set){EXTENSION|IMPLICIT_SRC|KEEP_DST,FLAGS,RCX      }; /* ror     r/m16/32/64 cl */
			extensions[2] = (struct x64_set){EXTENSION|IMPLICIT_SRC|KEEP_DST,FLAGS,FLAGS|RCX}; /* rcl     r/m16/32/64 cl */
			extensions[3] = (struct x64_set){EXTENSION|IMPLICIT_SRC|KEEP_DST,FLAGS,FLAGS|RCX}; /* rcr     r/m16/32/64 cl */
			extensions[4] = (struct x64_set){EXTENSION|IMPLICIT_SRC|KEEP_DST,FLAGS,RCX      }; /* shl/sal r/m16/32/64 cl */
			extensions[5] = (struct x64_set){EXTENSION|IMPLICIT_SRC|KEEP_DST,FLAGS,RCX      }; /* shr     r/m16/32/64 cl */
			extensions[6] = (struct x64_set){EXTENSION|IMPLICIT_SRC|KEEP_DST,FLAGS,RCX      }; /* sal/shl r/m16/32/64 cl */
			extensions[7] = (struct x64_set){EXTENSION|IMPLICIT_SRC|KEEP_DST,FLAGS,RCX      }; /* sar     r/m16/32/64 cl */
		}
		else if (opcode == 0xf6)
		{
			extensions[0] = (struct x64_set){EXTENSION|MOVE_DST                 ,FLAGS    ,  0}; /* test r/m8 imm8           */
			extensions[1] = (struct x64_set){EXTENSION|MOVE_DST                 ,FLAGS    ,  0}; /* test r/m8 imm8           */
			extensions[2] = (struct x64_set){EXTENSION                          ,        0,  0}; /* not  r/m8                */
			extensions[3] = (struct x64_set){EXTENSION                          ,FLAGS    ,  0}; /* neg  r/m8                */
			extensions[4] = (struct x64_set){EXTENSION|IMPLICIT_DST|IMPLICIT_SRC,FLAGS|RAX,RAX}; /* mul  ax   al   r/m8      */
			extensions[5] = (struct x64_set){EXTENSION|IMPLICIT_DST|IMPLICIT_SRC,FLAGS|RAX,RAX}; /* imul ax   al   r/m8      */
			extensions[6] = (struct x64_set){EXTENSION|IMPLICIT_DST|IMPLICIT_SRC,FLAGS|RAX,RAX}; /* div  al   ah   ax   r/m8 */
			extensions[7] = (struct x64_set){EXTENSION|IMPLICIT_DST|IMPLICIT_SRC,FLAGS|RAX|RDX,RAX|RDX}; /* idiv al   ah   ax   r/m8 */
		}
		else if (opcode == 0xf7)
		{
			extensions[0] = (struct x64_set){EXTENSION                                  ,FLAGS        ,      0}; /* test r/m16/32/64 imm16/32             */
			extensions[1] = (struct x64_set){EXTENSION                                  ,FLAGS        ,      0}; /* test r/m16/32/64 imm16/32             */
			extensions[2] = (struct x64_set){EXTENSION                                  ,            0,      0}; /* not  r/m16/32/64                      */
			extensions[3] = (struct x64_set){EXTENSION                                  ,FLAGS        ,      0}; /* neg  r/m16/32/64                      */
			extensions[4] = (struct x64_set){EXTENSION|IMPLICIT_DST|IMPLICIT_SRC        ,FLAGS|RAX|RDX,RAX|RDX}; /* mul  rDX         rAX      r/m16/32/64 */
			extensions[5] = (struct x64_set){EXTENSION|IMPLICIT_DST|IMPLICIT_SRC        ,FLAGS|RAX|RDX,RAX|RDX}; /* imul rDX         rAX      r/m16/32/64 */
			extensions[6] = (struct x64_set){EXTENSION|IMPLICIT_DST|EXT_SRC|IMPLICIT_SRC,FLAGS|RAX|RDX,RAX|RDX}; /* div  rDX         rAX      r/m16/32/64 */
			extensions[7] = (struct x64_set){EXTENSION|IMPLICIT_DST|EXT_SRC             ,FLAGS|RAX|RDX,RAX|RDX}; /* idiv rDX         rAX      r/m16/32/64 */
		}
		i = extensions[reg];
	}
	return i;
}

static struct x64_set	map_0f(uint8_t *p, uint8_t opcode, size_t codelen)
{
	struct x64_set	i;
	struct x64_set	instructions[0xff+1];

	bzero(instructions, sizeof(instructions));

	instructions[0x45] = (struct x64_set){MODRM               ,    0,FLAGS}; /* cmovnz/cmovne r16/32/64 r/m16/32/64 */
	instructions[0x92] = (struct x64_set){EXTENSION           ,    0,    0}; /*                                     */
	instructions[0x94] = (struct x64_set){EXTENSION           ,    0,    0}; /*                                     */
	instructions[0x95] = (struct x64_set){EXTENSION           ,    0,    0}; /*                                     */
	instructions[0x97] = (struct x64_set){EXTENSION           ,    0,    0}; /*                                     */
	instructions[0xaf] = (struct x64_set){MODRM|FLD_D|KEEP_DST,FLAGS,    0}; /* imul          r16/32/64 r/m16/32/64 */
	instructions[0xb6] = (struct x64_set){MODRM|FLD_D         ,    0,    0}; /* movzx         r16/32/64 r/m8        */
	instructions[0xb7] = (struct x64_set){MODRM|FLD_D         ,    0,    0}; /* movzx         r16/32/64 r/m16       */
	instructions[0xbe] = (struct x64_set){MODRM|FLD_D         ,    0,    0}; /* movsx         r16/32/64 r/m8        */

	i = instructions[opcode];

	if (i.status & EXTENSION)
	{
		if (!codelen--) return (struct x64_set){0,0,0}; /* error if instruction is too long */
		uint8_t		modrm = *p;
		uint8_t		reg   = (modrm & 0b00111000) >> 3;

		struct x64_set	extensions[8];

		if (opcode == 0x92)
		{
			extensions[0] = (struct x64_set){EXTENSION                ,0,FLAGS}; /* setb/setnae/setc r/m8 */
			extensions[1] = (struct x64_set){                          0,0,    0}; /* undefined             */
			extensions[2] = (struct x64_set){                          0,0,    0}; /* undefined             */
			extensions[3] = (struct x64_set){                          0,0,    0}; /* undefined             */
			extensions[4] = (struct x64_set){                          0,0,    0}; /* undefined             */
			extensions[5] = (struct x64_set){                          0,0,    0}; /* undefined             */
			extensions[6] = (struct x64_set){                          0,0,    0}; /* undefined             */
			extensions[7] = (struct x64_set){                          0,0,    0}; /* undefined             */
		}
		else if (opcode == 0x94)
		{
			extensions[0] = (struct x64_set){EXTENSION                ,0,FLAGS}; /* setz/sete r/m8   */
			extensions[1] = (struct x64_set){                          0,0,    0}; /* undefined        */
			extensions[2] = (struct x64_set){                          0,0,    0}; /* undefined        */
			extensions[3] = (struct x64_set){                          0,0,    0}; /* undefined        */
			extensions[4] = (struct x64_set){                          0,0,    0}; /* undefined        */
			extensions[5] = (struct x64_set){                          0,0,    0}; /* undefined        */
			extensions[6] = (struct x64_set){                          0,0,    0}; /* undefined        */
			extensions[7] = (struct x64_set){                          0,0,    0}; /* undefined        */
		}
		if (opcode == 0x95)
		{
			extensions[0] = (struct x64_set){EXTENSION                  ,0,FLAGS}; /* setnz/setne r/m8 */
			extensions[1] = (struct x64_set){                          0,0,    0}; /* undefined        */
			extensions[2] = (struct x64_set){                          0,0,    0}; /* undefined        */
			extensions[3] = (struct x64_set){                          0,0,    0}; /* undefined        */
			extensions[4] = (struct x64_set){                          0,0,    0}; /* undefined        */
			extensions[5] = (struct x64_set){                          0,0,    0}; /* undefined        */
			extensions[6] = (struct x64_set){                          0,0,    0}; /* undefined        */
			extensions[7] = (struct x64_set){                          0,0,    0}; /* undefined        */
		}
		else if (opcode == 0x97)
		{
			extensions[0] = (struct x64_set){EXTENSION                  ,0,FLAGS}; /* setnbe/seta r/m8 */
			extensions[1] = (struct x64_set){                          0,0,    0}; /* undefined        */
			extensions[2] = (struct x64_set){                          0,0,    0}; /* undefined        */
			extensions[3] = (struct x64_set){                          0,0,    0}; /* undefined        */
			extensions[4] = (struct x64_set){                          0,0,    0}; /* undefined        */
			extensions[5] = (struct x64_set){                          0,0,    0}; /* undefined        */
			extensions[6] = (struct x64_set){                          0,0,    0}; /* undefined        */
			extensions[7] = (struct x64_set){                          0,0,    0}; /* undefined        */
		}
		i = extensions[reg];
	}
	return i;
}

static void	memory_destination(uint32_t *src, uint32_t *dst)
{
	/* If destination is [reg], move reg to source and discard it
	** from destination. */
	if (src && dst && (*dst & MEMORY && *dst & 0xffff))
	{
		*src |= *dst & 0xffff;
		*dst &= ~(0xffff);
	}
}

static void	register_code(uint32_t *operand, uint32_t *flags,
			uint32_t reg_flag, uint32_t reg_ext)
{
	*flags   &= ~(reg_flag); /* clear old flag              */
	*operand |= reg_ext;     /* assigned new register flag  */
}

static bool	sib(struct safe_ptr ref, struct dsm dsm, uint8_t reg)
{
	if (!ref.size--) return false; /* error if instruction is too long */

	uint32_t	*dst      = dsm.dst;
	uint32_t	*src      = dsm.src;
	uint8_t		rex       = dsm.rex;

	uint8_t		opcode = *((uint8_t*)ref.ptr++);
	uint8_t		index  = (opcode & 0b00111000) >> 3;
	uint8_t		base   = (opcode & 0b00000111);

	/* MODRM.reg is the destination */
	if (src)
	{
		*src |= ENCODE_FLAG(index, REX_X(rex));
		*src |= ENCODE_FLAG(base, REX_B(rex));
	}
	if (dst)
		*dst |= ENCODE_FLAG(reg, REX_R(rex));

	return true;
}

static bool	direct_register(__unused struct safe_ptr ref, struct dsm dsm,
			uint8_t reg, uint8_t rm)
{
	uint32_t	*dst      = dsm.dst;
	uint32_t	*src      = dsm.src;
	uint8_t		rex       = dsm.rex;
	bool		direction = dsm.direction;

	if (src) *src |= direction ? ENCODE_FLAG(rm, REX_B(rex)) : ENCODE_FLAG(reg, REX_R(rex));
	if (dst) *dst |= direction ? ENCODE_FLAG(reg, REX_R(rex)) : ENCODE_FLAG(rm, REX_B(rex));

	return true;
}

static bool	indirect_register(struct safe_ptr ref, struct dsm dsm,
			uint8_t reg, uint8_t rm)
{
	uint32_t	*dst      = dsm.dst;
	uint32_t	*src      = dsm.src;
	uint8_t		rex       = dsm.rex;
	bool		direction = dsm.direction;

	if (rm == 0b100) /* SIB */
	{
		if (!sib(ref, dsm, reg)) return false;
	}
	else if (rm == 0b101) /* displacement only addressing [rip + disp] */
	{
		/* MODRM.reg is the destination */
		if (src) *src |= RIP;
		if (dst) *dst |= ENCODE_FLAG(reg, REX_R(rex));
	}
	else /* indirect register addressing */
	{
		/* MODRM.rm is dereferenced */
		if (src) *src |= direction ? ENCODE_FLAG(rm, REX_B(rex)) : ENCODE_FLAG(reg, REX_R(rex));
		if (dst) *dst |= direction ? ENCODE_FLAG(reg, REX_R(rex)) : ENCODE_FLAG(rm, REX_B(rex));
	}
	if (src) *src |= direction ? MEMORY : 0;
	if (dst) *dst |= direction ? 0 : MEMORY;

	return true;
}

static bool	indirect_register_displacement(struct safe_ptr ref, struct dsm dsm,
			uint8_t reg, uint8_t rm)
{
	uint32_t	*src      = dsm.src;
	uint32_t	*dst      = dsm.dst;
	uint8_t		rex       = dsm.rex;
	bool		direction = dsm.direction;

	if (rm == 0b100) /* SIB with displacement */
	{
		if (!sib(ref, dsm, reg)) return false;
	}
	else /* indirect register with displacement */
	{
		/* MODRM.rm is dereferenced */
		if (src) *src |= direction ? ENCODE_FLAG(rm, REX_B(rex)) : ENCODE_FLAG(reg, REX_R(rex));
		if (dst) *dst |= direction ? ENCODE_FLAG(reg, REX_R(rex)) : ENCODE_FLAG(rm, REX_B(rex));
	}
	if (src) *src |= direction ? MEMORY : 0;
	if (dst) *dst |= direction ? 0 : MEMORY;

	return true;
}

static bool	modrm(struct safe_ptr ref, struct dsm dsm)
{
	if (!ref.size--) return false; /* error if instruction is too long */

	uint8_t		opcode = *((uint8_t*)ref.ptr++);
	uint8_t		mod    = (opcode & 0b11000000) >> 6;
	uint8_t		reg    = (opcode & 0b00111000) >> 3;
	uint8_t		rm     = (opcode & 0b00000111);

	bool	(*addressing_mode[0b100])(struct safe_ptr, struct dsm, uint8_t, uint8_t);

	addressing_mode[0b00] = indirect_register;
	addressing_mode[0b01] = indirect_register_displacement;
	addressing_mode[0b10] = indirect_register_displacement;
	addressing_mode[0b11] = direct_register;

	return addressing_mode[mod](ref, dsm, reg, rm);
}

static void	dsm_instruction(uint8_t *code, size_t codelen,
			uint32_t *src, uint32_t *dst)
{
	uint8_t		prefix = 0;
	uint8_t		rex = 0;
	uint8_t		opcode;

	*src = UNKNOWN;
	*dst = UNKNOWN;

	/* set <codelen> to INSTRUCTION_MAXLEN if it exceeds it */
	if (codelen > INSTRUCTION_MAXLEN) codelen = INSTRUCTION_MAXLEN;

	/* prefix loop */
next_opcode:
	if (!codelen--) return ; /* error if instruction is too long */
	opcode = *code++;

	/* check if has prefix */
	if (opcode == 0x0f)
		{prefix |= OP_PREFIX_0F; goto next_opcode;}
	/* REX prefix(es) */
	else if ((opcode >= 0x40 && opcode <= 0x4f) && !prefix)
		{rex = opcode; goto next_opcode;}
	/* operand-size override */
	else if ((opcode == 0x66) & !prefix)
		{goto next_opcode;}

	/*
	** Choose between different tables dependent of the mapping
	** to retrieve informations about the instruction and
	** how it should be interpreted.
	*/
	struct x64_set	i = (struct x64_set){0,0,0};

	if      (prefix == 0)      {i = map_single(code, opcode, codelen);}
	else if (prefix == MAP_0F) {i = map_0f(code, opcode, codelen);}

	if (i.status == 0) return ; /* instruction is not supported */

	*src = RNO;
	*dst = RNO;

	/* d=1 memory to register (reg dest)   */
	/* d=0 register to memory (reg source) */
	bool	direction = i.status & FLD_D ? !!(opcode & 0b10) : 1;

	if (i.status & FLD_R)
	{
		/* register code added to primary opcode */
		uint8_t		reg_code = opcode & 0b111;
		uint32_t	reg_flag = ENCODE_FLAG(reg_code, 0);
		uint32_t	reg_ext  = ENCODE_FLAG(reg_code, REX_B(rex));

		if (i.status & PROMOTE_SRC && i.src_flags & reg_flag)
			register_code(src, &i.src_flags, reg_flag, reg_ext);
		if (i.status & PROMOTE_DST && i.dst_flags & reg_flag)
			register_code(dst, &i.dst_flags, reg_flag, reg_ext);
	}
	else if (i.status & EXTENSION)
	{
		if (i.status & EXT_SRC)
		{
			if (!modrm((struct safe_ptr){.ptr=code, .size=codelen},
				(struct dsm){.dst=NULL, .src=src, .rex=rex, .direction=direction}))
				return ;
		}
		else
		{
			if (!modrm((struct safe_ptr){.ptr=code, .size=codelen},
				(struct dsm){.dst=NULL, .src=dst, .rex=rex, .direction=direction}))
				return ;
		}
	}
	else if (i.status & MODRM)
	{
		if (!modrm((struct safe_ptr){.ptr=code, .size=codelen},
			(struct dsm){.dst=dst, .src=src, .rex=rex, .direction=direction}))
			return ;
	}

	memory_destination(src, dst);

	*src |= i.src_flags;                                                    /* get remaining flags                     */
	*dst |= i.dst_flags;                                                    /* get remaining flags                     */

	if (i.status & KEEP_DST) {*src |= *dst & (0xffff);}                     /* get register(s) from dst                */
	if (i.status & KEEP_SRC) {*dst |= *src & (0xffff);}                     /* get register(s) from src                */
	if (i.status & MOVE_DST)
	{
		*src |= *dst & (0xffff|MEMORY);
		*dst &= ~(0xffff|MEMORY);
	}
}

size_t		disasm_operands(struct operand *buf, size_t buflen,
			const void *code, size_t codelen)
{
	void		*p_code = (void*)code;
	struct operand	*p_buf  = buf;
	size_t		instruction_length = 0;

	while (codelen && buflen)
	{
		instruction_length = disasm_length(p_code, codelen);
		if (instruction_length == 0) return 0;

		dsm_instruction(p_code, codelen, &p_buf->src, &p_buf->dst);
		p_buf->addr   = p_code;
		p_buf->length = instruction_length;
		p_code  += instruction_length;
		codelen -= instruction_length;
		p_buf   += 1;
		buflen  -= 1;
	}
	return (p_buf - buf); /* number of instructions successfully disassembled */
}
