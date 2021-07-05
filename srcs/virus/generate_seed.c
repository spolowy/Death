
#include <stdbool.h>
#include <stdint.h>
#include <linux/elf.h>

#include "accessors.h"
#include "loader.h"
#include "utils.h"
#include "errors.h"
#include "log.h"

static bool	compute_elf_hdr_hash(uint64_t *hdr_hash, struct safe_ptr file_ref)
{
	Elf64_Ehdr	*elf_hdr = safe(file_ref, 0, sizeof(Elf64_Ehdr));

	if (elf_hdr == NULL) return errors(ERR_FILE, _ERR_CANT_READ_ELFHDR);

	*hdr_hash = hash((void *)elf_hdr, sizeof(Elf64_Ehdr));
	return true;
}

bool		generate_seed(uint64_t *seed, struct safe_ptr file_ref)
{
	uint64_t	father_seed = *seed;
	uint64_t	hdr_hash;

	if (!compute_elf_hdr_hash(&hdr_hash, file_ref))
		return errors(ERR_THROW, _ERR_IMPOSSIBLE);

	uint64_t	son_seed = father_seed ^ hdr_hash;

	son_seed = hash((void *)&son_seed, sizeof(son_seed));
	log_all_seeds(father_seed, hdr_hash, son_seed);

	*seed = son_seed;
	return true;
}
