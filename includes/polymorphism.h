/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   polymorphism.h                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anselme <anselme@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/12/11 20:01:16 by anselme           #+#    #+#             */
/*   Updated: 2020/03/01 18:19:32 by ichkamo          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef POLYMORPHISM_H
# define POLYMORPHISM_H

# include <stdbool.h>
# include <stdint.h>
# include <stddef.h>

uint64_t	polymorphic_seed_engine(uint64_t son_seed[2], uint64_t client_id);

bool		permutate_instructions(void *buffer, uint64_t seed, size_t size);
bool		permutate_registers(void *buffer, uint64_t seed, size_t size);
bool		permutate_blocks(void *code, uint64_t seed, size_t size);


#endif
