#ifndef UTILS_H
# define UTILS_H

# include <stdbool.h>
# include <stdint.h>
# include <sys/types.h>

# define POW2(n)	(1 << n)

void		bzero(void *ptr, size_t size);
void		*memset(void *ptr, uint8_t value, size_t size);
int		memcmp(const void *ptr1, const void *ptr2, size_t size);
void		*memcpy(void *dst, const void *src, size_t size);
size_t		strlen(const char *str);
char		*strcpy(char *dst, const char *src);
uint64_t	checksum(const uint8_t *ptr, size_t size);
uint64_t	hash(const uint8_t *ptr, size_t size);

uint64_t	random(uint64_t *seed);
uint64_t	random_inrange(uint64_t *seed, uint64_t lower, uint64_t upper);

# if defined(LOGS) || defined(ERRORS) || defined(DEBUG_BLOCKS) || defined(DEBUG_OPERANDS)

int		putchar(char chr);
int             putstr(const char *str);
void		putu64(uint64_t num);
void		puts32(int32_t num);
void		hexdump_text(const uint8_t *ptr, size_t size, size_t xsize);

# else

#  define putstr(...)
#  define putchar(...)
#  define putu64(...)
#  define puts32(...)
#  define hexdump_text(...)

# endif

#endif
