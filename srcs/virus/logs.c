#ifdef LOGS

/* --------------------------------- Colors --------------------------------- */

# define GREEN		'\033','[','0',';','3','2','m'
# define MAGENTA	'\033','[','0',';','3','5','m'
# define CYAN		'\033','[','0',';','3','6','m'
# define NONE		'\033','[','0','m'

/* -------------------------------------------------------------------------- */

# include "virus_header.h"
# include "utils.h"
# include "position_independent.h"

void	log_try_infecting(const char *file)
{
	PD_ARRAY(char,trying,CYAN,'[','L','O','G',']',' ','t','r','y','i','n','g',' ','t','o',' ','i','n','f','e','c','t',NONE,':',' ',0);

	putstr(trying);
	putstr(file);
	putchar('\n');
}

void	log_seeds(uint64_t father_seed, uint64_t client_id, uint64_t son_seed)
{
	PD_ARRAY(char, newline,NONE,'\n',0);
	PD_ARRAY(char,father,NONE,'[','L','O','G',']',' ','f','a','t','h','e','r','_','s','e','e','d',':',' ',' ',0);
	PD_ARRAY(char,client,NONE,'[','L','O','G',']',' ','c','l','i','e','n','t','_','i','d',':',' ',' ',' ',' ',0);
	PD_ARRAY(char,son,NONE,'[','L','O','G',']',' ','s','o','n','_','s','e','e','d',':',' ',' ',' ',' ',' ',MAGENTA,0);

	putstr(father);
	putu64(father_seed);
	putstr(newline);

	putstr(client);
	putu64(client_id);
	putstr(newline);

	putstr(son);
	putu64(son_seed);
	putstr(newline);
}

void	log_virus_size(size_t payload_size)
{
	PD_ARRAY(char,newline,NONE,'\n',0);
	PD_ARRAY(char,size,NONE,'[','L','O','G',']',' ','p','a','y','l','o','a','d','_','s','i','z','e',':',' ',0);

	putstr(size);
	puts32(payload_size);
	putstr(newline);

}

void	log_change_header_entry(void)
{
	PD_ARRAY(char,change_header_entry,NONE,'[','L','O','G',']',' ','e','n','t','r','y','_','p','o','i','n','t',':',' ',' ','e','_','e','n','t','r','y','\n',0);

	putstr(change_header_entry);
}

void	log_change_client_jump(void)
{
	PD_ARRAY(char,change_client_jump,NONE,'[','L','O','G',']',' ','e','n','t','r','y','_','p','o','i','n','t',':',' ',' ','c','l','i','e','n','t',' ','j','u','m','p','\n',0);

	putstr(change_client_jump);
}

void	log_success(void)
{
	PD_ARRAY(char, success,GREEN,'[','L','O','G',']',' ','i','n','f','e','c','t','i','o','n',' ','s','u','c','c','e','e','d','e','d',NONE,'\n',0);

	putstr(success);
}

#endif
