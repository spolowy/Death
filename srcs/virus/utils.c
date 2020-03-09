#include <sys/types.h>
#include <stdint.h>
#include <stddef.h>

#include "position_independent.h"
#include "syscall.h"

void	bzero(void *ptr, size_t size)
{
	char *tmp = ptr;

	for (size_t i = 0; i < size; i++)
		tmp[i] = 0;
}

int	memcmp(const void *s1, const void *s2, size_t n)
{
	size_t			i;
	const unsigned char	*ch_s1;
	const unsigned char	*ch_s2;

	ch_s1 = (const unsigned char*)s1;
	ch_s2 = (const unsigned char*)s2;
	i = 0;
	while (i < n)
	{
		if (ch_s1[i] != ch_s2[i])
			return (ch_s1[i] - ch_s2[i]);
		i++;
	}
	return (0);
}

size_t	strlen(const char *s)
{
	char	*p;

	p = (char*)s;
	while (*p)
		++p;
	return (p - s);
}

void	*memcpy(void *dst, void *src, size_t n)
{
	unsigned char *dest;
	unsigned char *source;

	dest = (unsigned char*)dst;
	source = (unsigned char*)src;
	while (n--)
	{
		*dest = *source;
		dest++;
		source++;
	}
	return dst;
}

char	*strcpy(char *dst, const char *src)
{
	int i = 0;

	while(src[i])
	{
		dst[i] = src[i];
		i++;
	}
	dst[i] = '\0';
	return dst;
}

void            *memset(void *b, int c, unsigned long len)
{
	unsigned long   m;
	unsigned char   *r;

	m = len >> 3;
	r = b + (m << 3);
	while (m--)
		((unsigned long*)b)[m] = c;
	len &= 7;
	while (len--)
		r[len] = c;
	return (b);
}

uint64_t	checksum(const char *buff, size_t buffsize)
{
	uint64_t	sum = 0;

	while (buffsize--)
		sum += buff[buffsize];
	return sum;
}

uint64_t	hash(const char *buff, size_t buffsize)
{
	uint64_t	state = 0xDEADC0DE;
	uint64_t	block;

	while (buffsize--)
	{
		block = buff[buffsize];
		state = (block * state) ^ ((block << 3) + (state >> 2));
	}
	return state;
}

#ifdef DEBUG

int             putchar(char c)
{
        return (sys_write(1, &c, 1));
}

int             putstr(const char *s)
{
        return (sys_write(1, s, strlen(s)));
}

void   		putu64(uint64_t n)
{
	PD_ARRAY(char, letter, '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F');

	if (n > 15)
	{
		putu64(n / 16);
		putu64(n % 16);
	}
	else
	{
		putchar(letter[n]);
	}
}

void   		dput32(int32_t n)
{
	PD_ARRAY(char, letter, '0', '1', '2', '3', '4', '5', '6', '7', '8', '9');

	if (n < 0)
	{
		putchar('-');
		n = n * -1;
	}
	if (n > 9)
	{
		dput32(n / 10);
		dput32(n % 10);
	}
	else
	{
		putchar(letter[n]);
	}
}

#endif
