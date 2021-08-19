#ifndef LOG_PRINT_OPERANDS
# define LOG_PRINT_OPERANDS

# ifdef DEBUG_OPERANDS

# include "disasm.h"
# include "utils.h"
# include "position_independent.h"

# define _print_masks(reg) ({                                                                              \
	PD_ARRAY(char,unknown,'\e','[','3','3','m','u','n','k','n','o','w','n',' ','\e','[','0','m',0)     \
	PD_ARRAY(char,rax,'\e','[','3','2','m','r','a','x',' ','\e','[','0','m',0)                         \
	PD_ARRAY(char,rbx,'\e','[','3','2','m','r','b','x',' ','\e','[','0','m',0)                         \
	PD_ARRAY(char,rcx,'\e','[','3','2','m','r','c','x',' ','\e','[','0','m',0)                         \
	PD_ARRAY(char,rdx,'\e','[','3','2','m','r','d','x',' ','\e','[','0','m',0)                         \
	PD_ARRAY(char,rdi,'\e','[','3','2','m','r','d','i',' ','\e','[','0','m',0)                         \
	PD_ARRAY(char,rsi,'\e','[','3','2','m','r','s','i',' ','\e','[','0','m',0)                         \
	PD_ARRAY(char,r8,'\e','[','3','2','m','r','8',' ','\e','[','0','m',0)                              \
	PD_ARRAY(char,r9,'\e','[','3','2','m','r','9',' ','\e','[','0','m',0)                              \
	PD_ARRAY(char,r10,'\e','[','3','2','m','r','1','0',' ','\e','[','0','m',0)                         \
	PD_ARRAY(char,r11,'\e','[','3','2','m','r','1','1',' ','\e','[','0','m',0)                         \
	PD_ARRAY(char,r12,'\e','[','3','2','m','r','1','2',' ','\e','[','0','m',0)                         \
	PD_ARRAY(char,r13,'\e','[','3','2','m','r','1','3',' ','\e','[','0','m',0)                         \
	PD_ARRAY(char,r14,'\e','[','3','2','m','r','1','4',' ','\e','[','0','m',0)                         \
	PD_ARRAY(char,r15,'\e','[','3','2','m','r','1','5',' ','\e','[','0','m',0)                         \
	PD_ARRAY(char,rbp,'\e','[','3','2','m','r','b','p',' ','\e','[','0','m',0)                         \
	PD_ARRAY(char,rsp,'\e','[','3','2','m','r','s','p',' ','\e','[','0','m',0)                         \
	PD_ARRAY(char,rip,'\e','[','3','2','m','r','i','p',' ','\e','[','0','m',0)                         \
	PD_ARRAY(char,memory,'\e','[','3','2','m','m','e','m','o','r','y',' ','\e','[','0','m',0)          \
	PD_ARRAY(char,flags,'\e','[','3','2','m','f','l','a','g','s',' ','\e','[','0','m',0)               \
	PD_ARRAY(char,none,'\e','[','3','2','m','n','o','n','e',' ','\e','[','0','m',0)                    \
	if (reg == (uint32_t)UNKNOWN)         {putstr(unknown);}                                           \
	else                                                                                               \
	{                                                                                                  \
		if (reg & RAX)                {putstr(rax);}                                               \
		if (reg & RBX)                {putstr(rbx);}                                               \
		if (reg & RCX)                {putstr(rcx);}                                               \
		if (reg & RDX)                {putstr(rdx);}                                               \
		if (reg & RDI)                {putstr(rdi);}                                               \
		if (reg & RSI)                {putstr(rsi);}                                               \
		if (reg & R8)                 {putstr(r8);}                                                \
		if (reg & R9)                 {putstr(r9);}                                                \
		if (reg & R10)                {putstr(r10);}                                               \
		if (reg & R11)                {putstr(r11);}                                               \
		if (reg & R12)                {putstr(r12);}                                               \
		if (reg & R13)                {putstr(r13);}                                               \
		if (reg & R14)                {putstr(r14);}                                               \
		if (reg & R15)                {putstr(r15);}                                               \
		if (reg & RBP)                {putstr(rbp);}                                               \
		if (reg & RSP)                {putstr(rsp);}                                               \
		if (reg & RIP)                {putstr(rip);}                                               \
		if (reg & MEMORY)             {putstr(memory);}                                            \
		if (reg & FLAGS)              {putstr(flags);}                                             \
		if (reg == RNO)               {putstr(none);}                                              \
	}                                                                                                  \
})

# define debug_print_operands(instructions, ninstructions) ({                                              \
	PD_ARRAY(char,nl,'\n',0);                                                                          \
	PD_ARRAY(char,hex,'0','x',0);                                                                      \
	PD_ARRAY(char,sep1,':','\t',0);                                                                    \
	PD_ARRAY(char,sep2,',',' ',0);                                                                     \
	for (size_t i = 0; i < ninstructions; i++)                                                         \
	{                                                                                                  \
		putstr(hex); putu64((size_t)instructions[i].addr);                                         \
		putstr(sep1);                                                                              \
		_print_masks(instructions[i].dst);                                                         \
		putstr(sep2);                                                                              \
		_print_masks(instructions[i].src);                                                         \
		putstr(nl);                                                                                \
	}                                                                                                  \
})

# else
#  define debug_print_operands(...)
# endif

#endif
