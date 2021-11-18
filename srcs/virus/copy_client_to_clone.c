#include "accessors.h"
#include "utils.h"
#include "errors.h"

static bool	copy_before_payload(struct safe_ptr clone_ref,
			struct safe_ptr file_ref, size_t payload_offset)
{
	const void	*original = safe(file_ref, 0, payload_offset);
	void		*clone    = safe(clone_ref, 0, payload_offset);

	if (original == NULL)
		return errors(ERR_FILE, _ERR_F_NO_ORIGINAL_FILE_BEGIN);
	if (clone == NULL)
		return errors(ERR_FILE, _ERR_F_NO_CLONE_FILE_BEGIN);

	memcpy(clone, original, payload_offset);
	return true;
}

static bool	copy_after_payload(struct safe_ptr clone_ref,
			struct safe_ptr file_ref, size_t payload_offset,
			size_t shift_amount)
{
	const size_t	remaining_size = file_ref.size - payload_offset;

	const void	*original = safe(file_ref, payload_offset, remaining_size);
	void		*clone    = safe(clone_ref, payload_offset + shift_amount, remaining_size);

	if (original == NULL)
		return errors(ERR_FILE, _ERR_F_NO_ORIGINAL_FILE_END);
	if (clone == NULL)
		return errors(ERR_FILE, _ERR_F_NO_CLONE_FILE_END);

	memcpy(clone, original, remaining_size);
	return true;
}

bool		copy_client_to_clone(struct safe_ptr clone_ref, struct safe_ptr file_ref,
			size_t payload_offset, size_t shift_amount)
{
	if (!copy_before_payload(clone_ref, file_ref, payload_offset)
	|| !copy_after_payload(clone_ref, file_ref, payload_offset, shift_amount))
	{
		return errors(ERR_THROW, _ERR_T_COPY_CLIENT_TO_CLONE);
	}
	return true;
}
