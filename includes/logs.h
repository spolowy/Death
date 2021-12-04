#ifndef LOG_H
# define LOG_H

# ifdef LOGS

#  include "virus_header.h"

void	log_try_infecting(const char *file);
void	log_seeds(uint64_t father_seed, uint64_t client_id, uint64_t son_seed);
void	log_virus_size(size_t payload_size);
void	log_change_header_entry(void);
void	log_change_client_jump(void);
void	log_success(void);

# else

#  define log_try_infecting(...)
#  define log_seeds(...)
#  define log_virus_size(...)
#  define log_change_header_entry(...)
#  define log_change_client_jump(...)
#  define log_success(...)

# endif

#endif
