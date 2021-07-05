
#include <stdint.h>

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

uint64_t	random_exrange(uint64_t *seed, uint64_t lower, uint64_t upper)
{
	return (random(seed) % (upper - lower - 1)) + lower + 1;
}
