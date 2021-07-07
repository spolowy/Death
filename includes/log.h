
#ifndef LOG_H
# define LOG_H

# ifdef DEBUG

# include "virus_header.h"

void	log_try_infecting(const char *file);
void	log_success(void);
void	log_all_seeds(const uint64_t father_seed, const uint64_t hdr_hash, const uint64_t son_seed);
void	log_virus_header(const struct virus_header *vhdr);

# else
#  define log_try_infecting(...)
#  define log_success(...)
#  define log_all_seeds(...)
#  define log_virus_header(...)
# endif

#endif
