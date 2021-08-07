#ifndef SYSCALL_H
# define SYSCALL_H

#include <linux/unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/ptrace.h>

/*
** struct dirent64 realistic reproduction
*/

struct dirent64
{
	__ino64_t		d_ino;
	__off64_t		d_off;
	unsigned short int	d_reclen;
	unsigned char		d_type;
	char			d_name[256];
};

/*
** syscalls
*/

ssize_t		sys_read(int fd, void *buf, size_t count);
ssize_t		sys_write(int fd, const void *buf, size_t count);
int		sys_open(const char *pathname, int flags, ...);
int		sys_close(int fd);
int		sys_fstat(int fd, struct stat *statbuf);
void		*sys_mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset);
int		sys_mprotect(void *addr, size_t len, int prot);
int		sys_munmap(void *addr, size_t length);
int     	sys_exit(int status);
long		sys_ptrace(enum __ptrace_request request, pid_t pid, void *addr, void *data);
int		sys_getdents64(unsigned int fd, struct dirent64 *dirp, unsigned int count);

#endif
