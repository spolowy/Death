/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: agrumbac <agrumbac@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/06/06 03:51:06 by agrumbac          #+#    #+#             */
/*   Updated: 2020/06/20 14:53:09 by ichkamo          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTILS_H
# define UTILS_H

# include <stdint.h>
# include <sys/types.h>

# define POW2(n)	(1 << n)

void		bzero(void *ptr, size_t size);
void		*memcpy(void *dst, void *src, size_t n);
int		memcmp(const void *s1, const void *s2, size_t n);
char		*strcpy(char *dst, const char *src);
size_t		strlen(const char *s);
void            *memset(void *b, int c, unsigned long len);

uint64_t	checksum(const char *buff, size_t buffsize);
uint64_t	hash(const char *buff, size_t buffsize);

uint64_t	random(uint64_t *seed);
uint64_t	random_inrange(uint64_t *seed, uint64_t lower, uint64_t upper);
uint64_t	random_exrange(uint64_t *seed, uint64_t lower, uint64_t upper);

# ifdef DEBUG

int		putstr(const char *str);
int		putchar(char c);
void		putu64(uint64_t n);
void		dput32(int32_t n);

# else

#  define hexdump_text(...)

# endif

#endif
