#include "accessors.h"
#include "virus.h"
#include "errors.h"

bool	foreach_phdr(struct safe_ptr ref, f_iter_callback callback, void *data)
{
	const Elf64_Ehdr	*elf_hdr = safe(ref, 0, sizeof(Elf64_Ehdr));

	if (elf_hdr == NULL)
		return errors(ERR_FILE, _ERR_F_READ_EHDR, _ERR_T_FOREACH_PHDR);

	const Elf64_Off		phoff     = elf_hdr->e_phoff;
	const uint16_t		phentsize = elf_hdr->e_phentsize;
	Elf64_Half		phnum     = elf_hdr->e_phnum;
	uint8_t			(*segments)[phnum][phentsize] = NULL;
	const size_t		array_size = phentsize * phnum;

	if (phentsize < sizeof(Elf64_Phdr)
	|| (array_size / phentsize != phnum)
	|| (!(segments = safe(ref, phoff, array_size))))
		return errors(ERR_FILE, _ERR_F_READ_PHDR, _ERR_T_FOREACH_PHDR);

	while (phnum--)
	{
		size_t	seg_hdr = (size_t)(*segments)[phnum];
		size_t	offset  = seg_hdr - (size_t)elf_hdr;

		if (!callback(ref, offset, data))
			return errors(ERR_THROW, _ERR_NO, _ERR_T_FOREACH_PHDR);
	}
	return true;
}

bool	foreach_shdr(struct safe_ptr ref, f_iter_callback callback, void *data)
{
	const Elf64_Ehdr	*elf_hdr = safe(ref, 0, sizeof(Elf64_Ehdr));

	if (elf_hdr == NULL)
		return errors(ERR_FILE, _ERR_F_READ_EHDR, _ERR_T_FOREACH_SHDR);

	const Elf64_Off		shoff     = elf_hdr->e_shoff;
	const uint16_t		shentsize = elf_hdr->e_shentsize;
	Elf64_Half		shnum     = elf_hdr->e_shnum;
	char			(*sections)[shnum][shentsize] = NULL;
	const size_t		array_size = shentsize * shnum;

	if (shentsize < sizeof(Elf64_Shdr)
	|| (array_size / shentsize != shnum)
	|| (!(sections = safe(ref, shoff, array_size))))
		return errors(ERR_FILE, _ERR_F_READ_SHDR, _ERR_T_FOREACH_SHDR);

	while (shnum--)
	{
		size_t	sect_hdr = (size_t)(*sections)[shnum];
		size_t	offset = sect_hdr - (size_t)elf_hdr;

		if (!callback(ref, offset, data))
			return errors(ERR_THROW, _ERR_NO, _ERR_T_FOREACH_SHDR);
	}
	return true;
}

bool	foreach_shdr_entry(struct safe_ptr ref, size_t offset, f_iter_callback callback, void *data)
{
	const Elf64_Ehdr	*ehdr = safe(ref, 0, sizeof(Elf64_Ehdr));
	const Elf64_Shdr	*shdr = safe(ref, offset, sizeof(Elf64_Shdr));

	if (ehdr == NULL || shdr == NULL)
		return errors(ERR_FILE, _ERR_F_READ_SHDR, _ERR_T_FOREACH_SHDR_ENTRY);

	const Elf64_Off	sh_offset  = shdr->sh_offset;
	const uint64_t	sh_entsize = shdr->sh_entsize;
	const uint64_t	sh_size    = shdr->sh_size;

	if (sh_entsize == 0)
		return true;

	size_t		entnum = sh_size / sh_entsize;
	uint8_t		(*entries)[entnum][sh_entsize] = safe(ref, sh_offset, sh_size);

	if (sh_size / sh_entsize != entnum || entries == NULL)
		return errors(ERR_FILE, _ERR_F_READ_SHDR, _ERR_T_FOREACH_SHDR_ENTRY);

	while (entnum--)
	{
		size_t	entry_addr   = (size_t)(*entries)[entnum];
		size_t	entry_offset = entry_addr - (size_t)ehdr;

		if (!callback(ref, entry_offset, data))
			return errors(ERR_THROW, _ERR_NO, _ERR_T_FOREACH_SHDR_ENTRY);
	}
	return true;
}
