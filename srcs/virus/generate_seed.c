/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   generate_seed.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anselme <anselme@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/12/12 18:29:06 by anselme           #+#    #+#             */
/*   Updated: 2021/06/15 17:38:57 by ichkamo          ###   ########.fr       */
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

static bool	compute_elf_hdr_hash(uint64_t *hdr_hash, struct safe_ptr ref)
{
	Elf64_Ehdr	*elf_hdr = safe(ref, 0, sizeof(Elf64_Ehdr));
	if (elf_hdr == NULL) return errors(ERR_FILE, _ERR_CANT_READ_ELFHDR);

	*hdr_hash = hash((void *)elf_hdr, sizeof(Elf64_Ehdr));
	return true;
}

bool		generate_seed(uint64_t *seed, struct safe_ptr original_ref)
{
	uint64_t	father_seed = *seed;
	uint64_t	hdr_hash;

	if (!compute_elf_hdr_hash(&hdr_hash, original_ref))
	// return errors(ERR_THROW, _ERR_GENERATE_SEED);
		return errors(ERR_THROW, _ERR_IMPOSSIBLE);

	uint64_t	son_seed = father_seed ^ hdr_hash;
	son_seed = hash((void *)&son_seed, sizeof(son_seed));
	log_all_seeds(father_seed, hdr_hash, son_seed);

	*seed = son_seed;
	return true;
}
