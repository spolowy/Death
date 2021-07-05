
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

# define JUMP32_INST_SIZE	5
# define CALL32_INST_SIZE	5

void		write_jump_arg(void *arg, int32_t value, uint8_t value_size);
bool		write_jump(void *buffer, int32_t value, uint8_t value_size);

# ifdef DEBUG

int		putstr(const char *str);
int		putchar(char c);
void		putu64(uint64_t n);
void		dput32(int32_t n);
void		hexdump_text(const uint8_t *text, size_t size, size_t xsize);

# else

#  define putstr(...)
#  define putchar(...)
#  define putu64(...)
#  define dput32(...)
#  define hexdump_text(...)

# endif

#endif
