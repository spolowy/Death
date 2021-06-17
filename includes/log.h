/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   log.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anselme <anselme@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/12/20 22:43:34 by anselme           #+#    #+#             */
/*   Updated: 2020/12/01 20:09:36 by ichkamo          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOG_H
# define LOG_H

# ifdef DEBUG

# include "loader.h"

void	log_try_infecting(const char *file);
void	log_success(void);
void	log_all_seeds(const uint64_t father_seed, const uint64_t hdr_hash, const uint64_t son_seed);
void	log_virus_header(struct virus_header *vhdr);

# else
#  define log_try_infecting(...)
#  define log_success(...)
#  define log_all_seeds(...)
#  define log_virus_header(...)
# endif

#endif
