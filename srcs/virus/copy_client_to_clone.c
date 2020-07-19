/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   copy_client_to_clone.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: agrumbac <agrumbac@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/05/13 14:58:36 by agrumbac          #+#    #+#             */
/*   Updated: 2020/06/20 21:31:55 by ichkamo          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "accessors.h"
#include "utils.h"
#include "errors.h"

static bool	copy_until_end_of_last_sect(struct safe_ptr clone_ref, struct safe_ptr original_ref, size_t end_of_last_section)
{
	void	*original = safe(original_ref, 0, end_of_last_section);
	void	*clone    = safe(clone_ref, 0, end_of_last_section);

	if (!original) return errors(ERR_FILE, _ERR_NO_ORIGINAL_FILE_BEGIN);
	if (!clone) return errors(ERR_FILE, _ERR_NO_CLONE_FILE_BEGIN);

	memcpy(clone, original, end_of_last_section);
	return true;
}

static bool	copy_after_payload(struct safe_ptr clone_ref, struct safe_ptr original_ref, \
			size_t end_last_sect, size_t shift_amount, size_t original_size)
{
	const size_t	size_after_last_sect = original_size - end_last_sect;

	void	*original = safe(original_ref, end_last_sect, size_after_last_sect);
	void	*clone    = safe(clone_ref, end_last_sect + shift_amount, size_after_last_sect);

	if (!original) return errors(ERR_FILE, _ERR_NO_ORIGINAL_FILE_END);
	if (!clone) return errors(ERR_FILE, _ERR_NO_CLONE_FILE_END);

	memcpy(clone, original, size_after_last_sect);
	return true;
}

bool		copy_client_to_clone(struct safe_ptr clone_ref, struct safe_ptr original_ref, \
			size_t end_last_sect, size_t shift_amount, size_t original_size)
{
	if (!copy_until_end_of_last_sect(clone_ref, original_ref, end_last_sect)
	|| !copy_after_payload(clone_ref, original_ref, end_last_sect, shift_amount, original_size))
	{
		return errors(ERR_THROW, _ERR_COPY_TO_CLONE);
	}
	return true;
}
