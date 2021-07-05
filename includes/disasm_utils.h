
#ifndef _DISASM_UTILS_H
# define _DISASM_UTILS_H

# include "disasm.h"

/*
** opcode prefixes
*/
# define OP_PREFIX_0F		(1 << 0)
// # define OP_PREFIX_66		(1 << 1)
// # define OP_PREFIX_F2		(1 << 2)
// # define OP_PREFIX_F3		(1 << 3)
// # define OP_PREFIX_9B		(1 << 4)
# define OP_PREFIX_REX		(1 << 5)
/*
** opcode mappings
*/
# define MAP_0F			OP_PREFIX_0F
// # define MAP_F3			OP_PREFIX_F3
// # define MAP_9B			OP_PREFIX_9B
// # define MAP_660F		(OP_PREFIX_66 | OP_PREFIX_0F)
// # define MAP_F20F		(OP_PREFIX_F2 | OP_PREFIX_0F)
// # define MAP_F30F		(OP_PREFIX_F3 | OP_PREFIX_0F)

# define TABLESIZE		8

# define BITMASK32(                                                            \
    b00,b01,b02,b03,b04,b05,b06,b07,                                           \
    b08,b09,b0a,b0b,b0c,b0d,b0e,b0f,                                           \
    b10,b11,b12,b13,b14,b15,b16,b17,                                           \
    b18,b19,b1a,b1b,b1c,b1d,b1e,b1f                                            \
) (                                                                            \
    (b00<<0x00)|(b01<<0x01)|(b02<<0x02)|(b03<<0x03)|                           \
    (b04<<0x04)|(b05<<0x05)|(b06<<0x06)|(b07<<0x07)|                           \
    (b08<<0x08)|(b09<<0x09)|(b0a<<0x0a)|(b0b<<0x0b)|                           \
    (b0c<<0x0c)|(b0d<<0x0d)|(b0e<<0x0e)|(b0f<<0x0f)|                           \
    (b10<<0x10)|(b11<<0x11)|(b12<<0x12)|(b13<<0x13)|                           \
    (b14<<0x14)|(b15<<0x15)|(b16<<0x16)|(b17<<0x17)|                           \
    (b18<<0x18)|(b19<<0x19)|(b1a<<0x1a)|(b1b<<0x1b)|                           \
    (b1c<<0x1c)|(b1d<<0x1d)|(b1e<<0x1e)|(b1f<<0x1f)                            \
)

# define CHECK_TABLE(t, v)   	((t[(v)>>5]>>((v)&0x1f))&1)

#endif
