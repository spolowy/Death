
#ifndef LOADER_H
# define LOADER_H

# include <stdbool.h>
# include <stdint.h>
# include <stddef.h>
# include <sys/types.h>

/*
** the virus header and its position in the loader's code
*/

struct			virus_header
{
	uint64_t	seed;
	size_t		full_virus_size;
	void		*loader_entry;
}__attribute__((packed));

void		virus_header_struct(void);

/*
** loader
*/

void		loader_entry(void);
void		call_virus(void);
void		jump_back_to_client(void);
void		loader_exit(void);

#endif
