#include <sys/types.h>
#include <stdint.h>
#include <stddef.h>
#include "syscalls.h"
#include "position_independent.h"

void		bzero(void *ptr, size_t size)
{
	uint8_t	*p = ptr;

	for (size_t i = 0; i < size; i++)
		p[i] = 0;
}

void		*memset(void *ptr, uint8_t value, size_t size)
{
	uint8_t	*p = ptr;

	for (size_t i = 0; i < size; i++)
		p[i] = value;

	return ptr;
}

int		memcmp(const void *ptr1, const void *ptr2, size_t size)
{
	const uint8_t	*p1 = ptr1;
	const uint8_t	*p2 = ptr2;

	for (size_t i = 0; i < size; i++)
	{
		if (p1[i] != p2[i])
			return (p1[i] - p2[i]);
	}
	return 0;
}

void		*memcpy(void *dst, const void *src, size_t size)
{
	const uint8_t	*psrc = src;
	uint8_t		*pdst = dst;

	for (size_t i = 0; i < size; i++)
	{
		pdst[i] = psrc[i];
	}
	return dst;
}

size_t		strlen(const char *str)
{
	const char	*p = str;

	while (*p) {p++;}

	return (p - str);
}

char		*strcpy(char *dst, const char *src)
{
	const char	*psrc = src;
	char		*pdst = dst;
	size_t		i = 0;

	while (psrc[i])
	{
		pdst[i] = psrc[i];
		i++;
	}
	pdst[i] = '\0';
	return dst;
}

uint64_t	checksum(const uint8_t *ptr, size_t size)
{
	uint64_t	sum = 0;

	while (size--)
		sum += ptr[size];

	return sum;
}

uint64_t	hash(const uint8_t *ptr, size_t size)
{
	uint64_t	state = 0xDEADC0DE;
	uint64_t	block;

	while (size--)
	{
		block = ptr[size];
		state = (block * state) ^ ((block << 3) + (state >> 2));
	}
	return state;
}

uint64_t	random(uint64_t *seed)
{
	uint64_t	rand = *seed;

	rand += 0xf0760a3c4;
	rand ^= rand << 13;
	rand ^= rand >> 17;
	rand -= 0x6fa624c2;
	rand ^= rand << 5;

	*seed = rand;
	return rand;
}

uint64_t	random_inrange(uint64_t *seed, uint64_t lower, uint64_t upper)
{
	return (random(seed) % (upper - lower + 1)) + lower;
}

#if defined(LOGS) || defined(ERRORS) || defined(DEBUG_BLOCKS) || defined(DEBUG_OPERANDS)

int		putchar(char chr)
{
	return sys_write(1, &chr, 1);
}

int             putstr(const char *str)
{
	return sys_write(1, str, strlen(str));
}

void		putu64(uint64_t num)
{
	PD_ARRAY(char, letter, '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F');

	if (num > 15)
	{
		putu64(num / 16);
		putu64(num % 16);
	}
	else
	{
		putchar(letter[num]);
	}
}

void		puts32(int32_t num)
{
	PD_ARRAY(char, letter, '0', '1', '2', '3', '4', '5', '6', '7', '8', '9');

	if (num < 0)
	{
		putchar('-');
		num = num * -1;
	}
	if (num > 9)
	{
		puts32(num / 10);
		puts32(num % 10);
	}
	else
	{
		putchar(letter[num]);
	}
}

void		hexdump_text(const uint8_t *ptr, size_t size, size_t xsize)
{
	PD_ARRAY(char,c_cyan,'\033','[','0',';','3','6','m',0);
	PD_ARRAY(char,c_none,'\033','[','0','m',0);
	PD_ARRAY(char,nl,'\n',0);
	PD_ARRAY(char,sp,' ',0);

	for (size_t i = 0; i < size; i += 0x10)
	{
		for (size_t j = 0; j < 0x10 && i + j < size; j++)
		{
			if (xsize)
			{
				putstr(c_cyan); putu64(ptr[i + j]); putstr(c_none);
				xsize--;
			}
			else
				putu64(ptr[i + j]);
			if (i + j < size)
				putstr(sp);
		}
		putstr(nl);
	}
}

#endif
