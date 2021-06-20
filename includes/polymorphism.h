/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   polymorphism.h                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anselme <anselme@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/12/11 20:01:16 by anselme           #+#    #+#             */
/*   Updated: 2020/12/01 14:44:43 by ichkamo          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef POLYMORPHISM_H
# define POLYMORPHISM_H

# include <stdbool.h>
# include <stdint.h>
# include <stddef.h>

bool		permutate_instructions(void *buffer, uint64_t seed, size_t size);
bool		permutate_registers(void *buffer, uint64_t seed, size_t size);
// bool		permutate_blocks(struct safe_ptr input_code, \
// 			struct safe_ptr output_buffer, \
// 			size_t *output_size, size_t *entry_point, \
// 			uint64_t seed);
bool		permutate_blocks(struct safe_ptr input_code, \
			struct safe_ptr output_buffer, \
			size_t *output_size,
			size_t virus_entry_point, int32_t *virus_func_shift, \
			uint64_t seed);

#endif
