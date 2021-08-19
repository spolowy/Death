#ifndef ERRORS_H
# define ERRORS_H

# ifdef ERRORS

#  include "utils.h"
#  include "position_independent.h"

/* ------------------------------ errors types ------------------------------ */

#  define ERR_SYS      PD_ARRAY(char,_err_type,'\033','[','3','1','m','s','y','s',':',' ',0)
#  define ERR_THROW    PD_ARRAY(char,_err_type,' ',' ','-','>',' ','i','n',' ','_',0)
#  define ERR_USAGE    PD_ARRAY(char,_err_type,'u','s','a','g','e',':',' ',0)
#  define ERR_FILE     PD_ARRAY(char,_err_type,'f','i','l','e',':',' ',0)
#  define ERR_VIRUS    PD_ARRAY(char,_err_type,'v','i','r','u','s',':',' ',0)

/* ------------------------------ errors usage ------------------------------ */

#  define _ERR_U_NOT_ELF                   PD_ARRAY(char,_err_text,'\033','[','3','3','m','n','o','t',' ','e','l','f','\n','\033','[','0','m',0)
#  define _ERR_U_NOT_A_FILE                PD_ARRAY(char,_err_text,'\033','[','3','3','m','n','o','t',' ','a',' ','f','i','l','e','\n','\033','[','0','m',0)

/* ----------------------------- errors system ------------------------------ */

#  define _ERR_S_FSTAT_FAILED              PD_ARRAY(char,_err_text,'\033','[','3','3','m','f','s','t','a','t',' ','f','a','i','l','e','d','\n','\033','[','0','m',0)
#  define _ERR_S_OPEN_FAILED               PD_ARRAY(char,_err_text,'\033','[','3','3','m','o','p','e','n',' ','f','a','i','l','e','d','\n','\033','[','0','m',0)
#  define _ERR_S_CLOSE_FAILED              PD_ARRAY(char,_err_text,'\033','[','3','3','m','c','l','o','s','e',' ','f','a','i','l','e','d','\n','\033','[','0','m',0)
#  define _ERR_S_MMAP_FAILED               PD_ARRAY(char,_err_text,'\033','[','3','3','m','m','m','a','p',' ','f','a','i','l','e','d','\n','\033','[','0','m',0)
#  define _ERR_S_MUNMAP_FAILED             PD_ARRAY(char,_err_text,'\033','[','3','3','m','m','u','n','m','a','p',' ','f','a','i','l','e','d','\n','\033','[','0','m',0)

/* ------------------------------- errors file ------------------------------ */

#  define _ERR_F_CANT_READ_ELFHDR          PD_ARRAY(char,_err_text,'\033','[','3','3','m','c','a','n','t',' ','r','e','a','d',' ','e','l','f','h','d','r','\n','\033','[','0','m',0)
#  define _ERR_F_NO_ENTRY_PHDR             PD_ARRAY(char,_err_text,'\033','[','3','3','m','c','o','u','l','d','n','\'','t',' ','f','i','n','d',' ','e','n','t','r','y',' ','p','h','d','r','\n','\033','[','0','m',0)
#  define _ERR_F_NO_ENTRY_SHDR             PD_ARRAY(char,_err_text,'\033','[','3','3','m','c','o','u','l','d','n','\'','t',' ','f','i','n','d',' ','e','n','t','r','y',' ','s','h','d','r','\n','\033','[','0','m',0)
#  define _ERR_F_NO_SECT_IN_ENTRY_SEG      PD_ARRAY(char,_err_text,'\033','[','3','3','m','n','o',' ','s','e','c','t',' ','i','n',' ','e','n','t','r','y','_','s','e','g','\n','\033','[','0','m',0)
#  define _ERR_F_BAD_PHDR_OFF              PD_ARRAY(char,_err_text,'\033','[','3','3','m','b','a','d',' ','p','h','d','r',' ','o','f','f','\n','\033','[','0','m',0)
#  define _ERR_F_BAD_SHDR_OFF              PD_ARRAY(char,_err_text,'\033','[','3','3','m','b','a','d',' ','s','h','d','r',' ','o','f','f','\n','\033','[','0','m',0)
#  define _ERR_F_NO_ORIGINAL_FILE_BEGIN    PD_ARRAY(char,_err_text,'\033','[','3','3','m','n','o',' ','o','r','i','g','i','n','a','l',' ','f','i','l','e',' ','b','e','g','i','n','n','i','n','g','\n','\033','[','0','m',0)
#  define _ERR_F_NO_CLONE_FILE_BEGIN       PD_ARRAY(char,_err_text,'\033','[','3','3','m','n','o',' ','c','l','o','n','e',' ','f','i','l','e',' ','b','e','g','i','n','n','i','n','g','\n','\033','[','0','m',0)
#  define _ERR_F_NO_ORIGINAL_FILE_END      PD_ARRAY(char,_err_text,'\033','[','3','3','m','n','o',' ','o','r','i','g','i','n','a','l',' ','f','i','l','e',' ','e','n','d','\n','\033','[','0','m',0)
#  define _ERR_F_NO_CLONE_FILE_END         PD_ARRAY(char,_err_text,'\033','[','3','3','m','n','o',' ','c','l','o','n','e',' ','f','i','l','e',' ','e','n','d','\n','\033','[','0','m',0)

/* ------------------------------ errors throws ----------------------------- */

#  define _ERR_T_INFECT                    PD_ARRAY(char,_err_text,'\033','[','3','3','m','i','n','f','e','c','t','\n','\033','[','0','m',0)
#  define _ERR_T_WRITE_FILE                PD_ARRAY(char,_err_text,'\033','[','3','3','m','w','r','i','t','e','_','f','i','l','e','\n','\033','[','0','m',0)
#  define _ERR_T_INFECTION_ENGINE          PD_ARRAY(char,_err_text,'\033','[','3','3','m','i','n','f','e','c','t','i','o','n','_','e','n','g','i','n','e','\n','\033','[','0','m',0)
#  define _ERR_T_FIND_ENTRY                PD_ARRAY(char,_err_text,'\033','[','3','3','m','f','i','n','d','_','e','n','t','r','y','\n','\033','[','0','m',0)
#  define _ERR_T_FOREACH_SHDR              PD_ARRAY(char,_err_text,'\033','[','3','3','m','f','o','r','e','a','c','h','_','s','h','d','r','\n','\033','[','0','m',0)
#  define _ERR_T_FOREACH_PHDR              PD_ARRAY(char,_err_text,'\033','[','3','3','m','f','o','r','e','a','c','h','_','p','h','d','r','\n','\033','[','0','m',0)
#  define _ERR_T_GENERATE_SEED             PD_ARRAY(char,_err_text,'\033','[','3','3','m','g','e','n','e','r','a','t','e','_','s','e','e','d','\n','\033','[','0','m',0)
#  define _ERR_T_METAMORPH_CLONE           PD_ARRAY(char,_err_text,'\033','[','3','3','m','e','t','a','m','o','r','p','h','_','c','l','o','n','e','\n','\033','[','0','m',0)
#  define _ERR_T_PERMUTATE_REGISTERS       PD_ARRAY(char,_err_text,'\033','[','3','3','m','p','e','r','m','u','t','a','t','e','_','r','e','g','i','s','t','e','r','s','\n','\033','[','0','m',0)
#  define _ERR_T_EXTENDED_OPCODE           PD_ARRAY(char,_err_text,'\033','[','3','3','m','e','x','t','e','n','d','e','d','_','o','p','c','o','d','e','\n','\033','[','0','m',0)
#  define _ERR_T_MODRM                     PD_ARRAY(char,_err_text,'\033','[','3','3','m','m','o','d','r','m','\n','\033','[','0','m',0)
#  define _ERR_T_PERMUTATE_BLOCKS          PD_ARRAY(char,_err_text,'\033','[','3','3','m','p','e','r','m','u','t','a','t','e','_','b','l','o','c','k','s','\n','\033','[','0','m',0)
#  define _ERR_T_PERMUTATE_INSTRUCTIONS    PD_ARRAY(char,_err_text,'\033','[','3','3','m','p','e','r','m','u','t','a','t','e','_','i','n','s','t','r','u','c','t','i','o','n','s','\n','\033','[','0','m',0)
#  define _ERR_T_DISASM_BLOCK              PD_ARRAY(char,_err_text,'\033','[','3','3','m','d','i','s','a','s','m','_','b','l','o','c','k','\n','\033','[','0','m',0)
#  define _ERR_T_DISASM_JUMPS              PD_ARRAY(char,_err_text,'\033','[','3','3','m','d','i','s','a','s','m','_','j','u','m','p','s','\n','\033','[','0','m',0)
#  define _ERR_T_SHARD_BLOCK               PD_ARRAY(char,_err_text,'\033','[','3','3','m','s','h','a','r','d','_','b','l','o','c','k','\n','\033','[','0','m',0)
#  define _ERR_T_RECURSIVE_SPLIT_BLOCKS    PD_ARRAY(char,_err_text,'\033','[','3','3','m','r','e','c','u','r','s','i','v','e','_','s','p','l','i','t','_','b','l','o','c','k','s','\n','\033','[','0','m',0)
#  define _ERR_T_SPLIT_BLOCK               PD_ARRAY(char,_err_text,'\033','[','3','3','m','s','p','l','i','t','_','b','l','o','c','k','\n','\033','[','0','m',0)
#  define _ERR_T_WRITE_PERMUTATED_CODE     PD_ARRAY(char,_err_text,'\033','[','3','3','m','w','r','i','t','e','_','p','e','r','m','u','t','a','t','e','d','_','c','o','d','e','\n','\033','[','0','m',0)
#  define _ERR_T_COPY_CLIENT_TO_CLONE      PD_ARRAY(char,_err_text,'\033','[','3','3','m','c','o','p','y','_','c','l','i','e','n','t','_','t','o','_','c','l','o','n','e','\n','\033','[','0','m',0)
#  define _ERR_T_ADJUST_REFERENCES         PD_ARRAY(char,_err_text,'\033','[','3','3','m','a','d','j','u','s','t','_','r','e','f','e','r','e','n','c','e','s','\n','\033','[','0','m',0)

/* ------------------------------ errors virus ------------------------------ */

#  define _ERR_V_ALREADY_INFECTED          PD_ARRAY(char,_err_text,'\033','[','3','2','m','a','l','r','e','a','d','y',' ','i','n','f','e','c','t','e','d',' ','<','3','\n','\033','[','0','m',0)
#  define _ERR_V_CANT_READ_SIGNATURE       PD_ARRAY(char,_err_text,'\033','[','3','2','m','c','a','n','\'','t',' ','r','e','a','d',' ','s','i','g','n','a','t','u','r','e','\n','\033','[','0','m',0)
#  define _ERR_V_CANT_READ_FULL_VIRUS      PD_ARRAY(char,_err_text,'\033','[','3','2','m','c','a','n','\'','t',' ','r','e','a','d',' ','f','u','l','l',' ','v','i','r','u','s','\n','\033','[','0','m',0)
#  define _ERR_V_COPY_VIRUS_TO_CLONE       PD_ARRAY(char,_err_text,'\033','[','3','3','m','c','o','p','y','_','l','o','a','d','e','r','_','t','o','_','c','l','o','n','e','\n','\033','[','0','m',0)
#  define _ERR_V_CANT_READ_LOADER          PD_ARRAY(char,_err_text,'\033','[','3','3','m','c','a','n','\'','t',' ','r','e','a','d',' ','l','o','a','d','e','r','\n','\033','[','0','m',0)
#  define _ERR_V_CANT_READ_VIRUS           PD_ARRAY(char,_err_text,'\033','[','3','3','m','c','a','n','\'','t',' ','r','e','a','d',' ','v','i','r','u','s','\n','\033','[','0','m',0)
#  define _ERR_V_CANT_READ_VIRCALL         PD_ARRAY(char,_err_text,'\033','[','3','3','m','c','a','n','\'','t',' ','r','e','a','d',' ','v','i','r','c','a','l','l','\n','\033','[','0','m',0)
#  define _ERR_V_MAX_JUMP_TOO_SMALL        PD_ARRAY(char,_err_text,'\033','[','3','3','m','m','a','x',' ','j','u','m','p','s',' ','t','o','o',' ','s','m','a','l','l','\n','\033','[','0','m',0)
#  define _ERR_V_OPERANDS_JUMP_TOO_SMALL   PD_ARRAY(char,_err_text,'\033','[','3','3','m','m','a','x',' ','o','p','e','r','a','n','d','s',' ','t','o','o',' ','s','m','a','l','l','\n','\033','[','0','m',0)
#  define _ERR_V_INSTRUCTION_LENGTH        PD_ARRAY(char,_err_text,'\033','[','3','3','m','i','n','s','t','r','u','c','t','i','o','n',' ','l','e','n','g','t','h',' ','e','x','c','e','e','d','s',' ','c','o','d','e',' ','l','e','n','g','t','h','\n','\033','[','0','m',0)
#  define _ERR_V_CANT_SPLIT_MORE           PD_ARRAY(char,_err_text,'\033','[','3','3','m','c','a','n','\'','t',' ','s','p','l','i','t',' ','m','o','r','e','\n','\033','[','0','m',0)
#  define _ERR_V_NO_ENTRY_POINT            PD_ARRAY(char,_err_text,'\033','[','3','3','m','c','o','u','l','d','n','\'','t',' ','f','i','n','d',' ','e','n','t','r','y',' ','p','o','i','n','t','\n','\033','[','0','m',0)
#  define _ERR_V_CANT_READ_BLOCK           PD_ARRAY(char,_err_text,'\033','[','3','3','m','c','a','n','\'','t',' ','r','e','a','d',' ','b','l','o','c','k','\n','\033','[','0','m',0)
#  define _ERR_V_ADJUST_JUMPS              PD_ARRAY(char,_err_text,'\033','[','3','3','m','a','d','j','u','s','t',' ','j','u','m','p','s','\n','\033','[','0','m',0)
#  define _ERR_V_ADD_TRAILING_JUMP         PD_ARRAY(char,_err_text,'\033','[','3','3','m','a','d','d',' ','t','r','a','i','l','i','n','g',' ','j','u','m','p','\n','\033','[','0','m',0)
#  define _ERR_V_BAD_TRAILING_JUMP         PD_ARRAY(char,_err_text,'\033','[','3','3','m','t','r','a','i','l','i','n','g',' ','j','u','m','p',' ','v','a','l','u','e',' ','d','o','e','s','n','\'','t',' ','m','a','t','c','h',' ','d','e','s','t','i','n','a','t','i','o','n','\n','\033','[','0','m',0)
#  define _ERR_V_NOT_ENOUGH_PADDING        PD_ARRAY(char,_err_text,'\033','[','3','3','m','n','o','t',' ','e','n','o','u','g','h',' ','p','a','d','d','i','n','g','\n','\033','[','0','m',0)
#  define _ERR_V_CANT_READ_CONSTANTS       PD_ARRAY(char,_err_text,'\033','[','3','3','m','c','a','n','t','\'','t',' ','r','e','a','d',' ','c','o','n','s','t','a','n','t','s','\n','\033','[','0','m',0)

/* -------------------------------------------------------------------------- */

#  define errors(err_type, err_text)		({ \
	err_type;                                  \
	err_text;                                  \
	putstr(_err_type);                         \
	putstr(_err_text);                         \
	(false);                                   \
})

# else
#  define errors(...)		false
# endif

#endif
