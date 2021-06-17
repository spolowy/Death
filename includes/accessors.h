/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   accessors.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: agrumbac <agrumbac@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/06/04 04:47:10 by agrumbac          #+#    #+#             */
/*   Updated: 2021/06/15 16:25:03 by ichkamo          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ACCESSORS_H
# define ACCESSORS_H

#include <sys/types.h>
#include <stdbool.h>
#include <fcntl.h>
#include <unistd.h>

/*
** Safe pointers
** - non NULL
** - immutable
** - points to valid memory
*/

struct	safe_ptr
{
	void	*ptr;
	size_t	size;
};

void	*safe(struct safe_ptr ref, size_t offset, size_t size);
bool	free_accessor(struct safe_ptr *ref);
bool	write_file(struct safe_ptr ref, const char *filename);

/*
** Original and clone safe_ptr initializer
*/

bool	init_original_safe(struct safe_ptr *ref, const char *filename);
bool	init_clone_safe(struct safe_ptr *accessor, size_t original_filesize, \
		size_t virus_size);

#endif
