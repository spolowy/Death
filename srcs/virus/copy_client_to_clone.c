#include "accessors.h"
#include "utils.h"
#include "errors.h"

static bool	copy_until_end_of_last_sect(struct safe_ptr clone_ref,
			struct safe_ptr file_ref, size_t end_of_last_section)
{
	const void	*original = safe(file_ref, 0, end_of_last_section);
	void		*clone    = safe(clone_ref, 0, end_of_last_section);

	if (original == NULL)
		return errors(ERR_FILE, _ERR_F_NO_ORIGINAL_FILE_BEGIN);
	if (clone == NULL)
		return errors(ERR_FILE, _ERR_F_NO_CLONE_FILE_BEGIN);

	memcpy(clone, original, end_of_last_section);
	return true;
}

static bool	copy_after_payload(struct safe_ptr clone_ref,
			struct safe_ptr file_ref, size_t end_of_last_section,
			size_t shift_amount)
{
	const size_t	size_after_last_sect = file_ref.size - end_of_last_section;

	const void	*original = safe(file_ref, end_of_last_section, size_after_last_sect);
	void		*clone    = safe(clone_ref, end_of_last_section + shift_amount, size_after_last_sect);

	if (original == NULL)
		return errors(ERR_FILE, _ERR_F_NO_ORIGINAL_FILE_END);
	if (clone == NULL)
		return errors(ERR_FILE, _ERR_F_NO_CLONE_FILE_END);

	memcpy(clone, original, size_after_last_sect);
	return true;
}

bool		copy_client_to_clone(struct safe_ptr clone_ref, struct safe_ptr file_ref,
			size_t end_of_last_section, size_t shift_amount)
{
	if (!copy_until_end_of_last_sect(clone_ref, file_ref, end_of_last_section)
	|| !copy_after_payload(clone_ref, file_ref, end_of_last_section, shift_amount))
	{
		return errors(ERR_THROW, _ERR_T_COPY_CLIENT_TO_CLONE);
	}
	return true;
}
