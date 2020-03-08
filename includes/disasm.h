/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   disasm.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anselme <anselme@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/12/14 18:48:28 by anselme           #+#    #+#             */
/*   Updated: 2020/02/22 22:36:11 by ichkamo          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef DISASM_H
# define DISASM_H

# include <sys/types.h>
# include <stdint.h>
# include <stdbool.h>

/*
** Flags used to specify operand or status.
** Although GP registers can have a 8-bit, 16-bit or 32-bit size we consider
** for this disassembler that the whole 64-bit register is affected.
*/
# define RNO		(0)       /* nothing                            */
# define RAX		(1 << 0)  /* al   || ax   || eax  || rax        */
# define RCX		(1 << 1)  /* cl   || cx   || ecx  || rcx        */
# define RDX		(1 << 2)  /* dl   || dx   || edx  || rdx        */
# define RBX		(1 << 3)  /* bl   || bx   || ebx  || rbx        */
# define RSP		(1 << 4)  /* ah   || spl  || sp   || esp || rsp */
# define RBP		(1 << 5)  /* ch   || bpl  || bp   || ebp || rbp */
# define RSI		(1 << 6)  /* dh   || sil  || si   || esi || rsi */
# define RDI		(1 << 7)  /* bh   || dil  || di   || edi || rdi */
# define R8		(1 << 8)  /* r8l  || r8w  || r8d  || r8         */
# define R9		(1 << 9)  /* r9l  || r9w  || r9d  || r9         */
# define R10		(1 << 10) /* r10l || r10w || r10d || r10        */
# define R11		(1 << 11) /* r11l || r11w || r11d || r11        */
# define R12		(1 << 12) /* r12l || r12w || r12d || r12        */
# define R13		(1 << 13) /* r13l || r13w || r13d || r13        */
# define R14		(1 << 14) /* r14l || r14w || r14d || r14        */
# define R15		(1 << 15) /* r15l || r15w || r15d || r15        */

# define RIP		(1 << 16) /* eip  || rip                        */

# define FLAGS		(1 << 30) /* uses or modifies flags             */
# define MEMORY		(1 << 31) /* references a memory location       */
# define UNKNOWN	(~0)      /* unknown modification(s)            */
/*
** Maximum instruction length in byte x86 can interpret.
** Otherwise an invalid opcode exception is thrown.
*/
# define INSTRUCTION_MAXLEN	15

/*
** structure of a disassembled instruction
*/
struct operands
{
	void		*addr;		/* instruction address   */
	size_t		length;		/* instruction length    */
	uint32_t	src;		/* source operand        */
	uint32_t	dst;		/* destination operand   */
};

/*
** structure of a disassembled control flow instruction
*/
struct control_flow
{
	void            *addr;        /* instruction address             */
	size_t          length;       /* instruction length              */
	void            *value_addr;  /* value address in instruction    */
	int8_t          value_length; /* value size in byte              */
	int32_t         value;        /* immediate value in instruction  */
	void            *label_addr;  /* label address                   */
};

/*
** disassembly functions
*/
size_t		disasm_length(const void *code, size_t codelen);
size_t		disasm_operands(const void *code, size_t codelen,
			struct operands *buf, size_t buflen);
size_t		disasm_jumps(struct control_flow *buf, size_t buflen,
			const void *code, size_t codelen);

#endif
