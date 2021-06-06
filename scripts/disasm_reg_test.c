// gcc disasm_reg_test.c -I ../includes/ ../srcs/virus/disasm.c ../srcs/virus/disasm_length.c && ./a.out

# define NONE		(0)       /* nothing                            */
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

# define STACK_FRAME 4

#include "disasm.h"
#include "disasm_utils.h"
#include "compiler_utils.h"

struct reg_tests {
	uint8_t		*code;
	uint32_t	src;		/* source operand        */
	uint32_t	dst;		/* destination operand   */
};

void mov_drax_rcx() {asm volatile (".intel_syntax\n mov [rax], rcx\n");}
void mov_rax_drcx() {asm volatile (".intel_syntax\n mov rax, [rcx]\n");}
void push_rbp()     {asm volatile (".intel_syntax\n push rbp\n");}
// add more instructions here

struct reg_tests tests[] = {
	{.code=mov_drax_rcx, .src=RAX|RCX, .dst=MEMORY},
	{.code=mov_rax_drcx, .src=MEMORY|RCX, .dst=RAX},
	{.code=push_rbp,     .src=RBP|RSP, .dst=RSP|MEMORY}
	// add more tests here
};

int	main(void)
{
	struct s_instruction inst;

	for (size_t i = 0; i < ARRAY_SIZE(tests); i++)
	{
		size_t ret = disasm(tests[i].code + STACK_FRAME, 15, &inst, 1);
		if (ret == 1
		&& inst.src == tests[i].src 
		&& inst.dst == tests[i].dst)
			printf("ok\n");
		else
			printf("oh no!\n");
	}
}
