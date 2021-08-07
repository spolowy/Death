#ifndef COMPILER_UTILS_H
# define COMPILER_UTILS_H

# define __unused			__attribute__((unused))
# define __noreturn			__attribute__((noreturn))
# define __warn_unused_result		__attribute__((warn_unused_result))
# define __nonull			__attribute__((nonnull))

# define ARRAY_SIZE(x)			(sizeof(x) / sizeof(*x))

# define PAGE_ALIGNMENT			4096
# define ALIGN(x, n)			(((x) + (n)) & ~((n) - 1))

# define BYTE		1
# define WORD		2
# define DWORD		4
# define PWORD		6
# define QWORD		8
# define TWORD		10

#endif
