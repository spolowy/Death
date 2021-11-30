#ifndef VIRUS_HEADER_H
# define VIRUS_HEADER_H

# include <stdint.h>
# include <stddef.h>
# include <sys/types.h>

struct virus_header
{
	uint64_t	seed;
	size_t		full_virus_size;
	void		*loader_entry;
	size_t		loader_size;
	size_t		dist_virus_loader;
	size_t		dist_vircall_loader;
	size_t		dist_header_loader;
	size_t		dist_jmpclient_loader;
};

#endif
