#ifndef ERRORS_H
# define ERRORS_H

# ifdef ERRORS

#  include "utils.h"
#  include "position_independent.h"

/* --------------------------------- colors --------------------------------- */

# define GREEN         '\033','[','0',';','3','2','m'
# define YELLOW        '\033','[','0',';','3','3','m'
# define NONE          '\033','[','0','m'

/* ------------------------------ errors types ------------------------------ */

#  define ERR_THROW                         PD_ARRAY(char,_err_type,' ',' ','-','>',' ','i','n',' ','_',0)
#  define ERR_USAGE                         PD_ARRAY(char,_err_type,'u','s','a','g','e',':',' ',0)
#  define ERR_FILE                          PD_ARRAY(char,_err_type,'f','i','l','e',':',' ',0)
#  define ERR_SYS                           PD_ARRAY(char,_err_type,'s','y','s',':',' ',0)
#  define ERR_VIRUS                         PD_ARRAY(char,_err_type,'v','i','r','u','s',':',' ',0)

/* ----------------------------- no error text ------------------------------ */

#  define _ERR_NO                           PD_ARRAY(char,_err_text,0)

/* ------------------------------ errors throws ----------------------------- */

#  define _ERR_T_FREE_ACCESSOR              PD_ARRAY(char,_func_name,YELLOW,'f','r','e','e','_','a','c','c','e','s','s','o','r','\n',NONE,0)
#  define _ERR_T_INIT_FILE_SAFE             PD_ARRAY(char,_func_name,YELLOW,'i','n','i','t','_','f','i','l','e','_','s','a','f','e','\n',NONE,0)
#  define _ERR_T_INIT_CLONE_SAFE            PD_ARRAY(char,_func_name,YELLOW,'i','n','i','t','_','c','l','o','n','e','_','s','a','f','e','\n',NONE,0)
#  define _ERR_T_WRITE_FILE                 PD_ARRAY(char,_func_name,YELLOW,'w','r','i','t','e','_','f','i','l','e','\n',NONE,0)
#  define _ERR_T_ADJUST_REFERENCES          PD_ARRAY(char,_func_name,YELLOW,'a','d','j','u','s','t','_','r','e','f','e','r','e','n','c','e','s','\n',NONE,0)
#  define _ERR_T_ADJUST_HEADER_VALUES       PD_ARRAY(char,_func_name,YELLOW,'a','d','j','u','s','t','_','h','e','a','d','e','r','_','v','a','l','u','e','s','\n',NONE,0)
#  define _ERR_T_ADJUST_PHDR_VALUES         PD_ARRAY(char,_func_name,YELLOW,'a','d','j','u','s','t','_','p','h','d','r','_','v','a','l','u','e','s','\n',NONE,0)
#  define _ERR_T_ADJUST_SHDR_VALUES         PD_ARRAY(char,_func_name,YELLOW,'a','d','j','u','s','t','_','s','h','d','r','_','v','a','l','u','e','s','\n',NONE,0)
#  define _ERR_T_ADJUST_DYNSYM_VALUES       PD_ARRAY(char,_func_name,YELLOW,'a','d','j','u','s','t','_','d','y','n','s','y','m','_','v','a','l','u','e','s','\n',NONE,0)
#  define _ERR_T_ADJUST_REL_VALUES          PD_ARRAY(char,_func_name,YELLOW,'a','d','j','u','s','t','_','r','e','l','_','v','a','l','u','e','s','\n',NONE,0)
#  define _ERR_T_ADJUST_RELA_VALUES         PD_ARRAY(char,_func_name,YELLOW,'a','d','j','u','s','t','_','r','e','l','a','_','v','a','l','u','e','s','\n',NONE,0)
#  define _ERR_T_ADJUST_DYNAMIC_VALUES      PD_ARRAY(char,_func_name,YELLOW,'a','d','j','u','s','t','_','d','y','n','a','m','i','c','_','v','a','l','u','e','s','\n',NONE,0)
#  define _ERR_T_CHANGE_ENTRY               PD_ARRAY(char,_func_name,YELLOW,'c','h','a','n','g','e','_','e','n','t','r','y','\n',NONE,0)
#  define _ERR_T_CHANGE_CLIENT_JUMP         PD_ARRAY(char,_func_name,YELLOW,'c','h','a','n','g','e','_','c','l','i','e','n','t','_','j','u','m','p','\n',NONE,0)
#  define _ERR_T_CHANGE_HEADER_ENTRY        PD_ARRAY(char,_func_name,YELLOW,'c','h','a','n','g','e','_','h','e','a','d','e','r','_','e','n','t','r','y','\n',NONE,0)
#  define _ERR_T_COPY_VIRUS_TO_CLONE        PD_ARRAY(char,_func_name,YELLOW,'c','o','p','y','_','v','i','r','u','s','_','t','o','_','c','l','o','n','e','\n',NONE,0)
#  define _ERR_T_COPY_CLIENT_TO_CLONE       PD_ARRAY(char,_func_name,YELLOW,'c','o','p','y','_','c','l','i','e','n','t','_','t','o','_','c','l','o','n','e','\n',NONE,0)
#  define _ERR_T_COPY_BEFORE_PAYLOAD        PD_ARRAY(char,_func_name,YELLOW,'c','o','p','y','_','b','e','f','o','r','e','_','p','a','y','l','o','a','d','\n',NONE,0)
#  define _ERR_T_COPY_AFTER_PAYLOAD         PD_ARRAY(char,_func_name,YELLOW,'c','o','p','y','_','a','f','t','e','r','_','p','a','y','l','o','a','d','\n',NONE,0)
#  define _ERR_T_DISASM_BLOCK               PD_ARRAY(char,_func_name,YELLOW,'d','i','s','a','s','m','_','b','l','o','c','k','\n',NONE,0)
#  define _ERR_T_DISASM_JUMPS               PD_ARRAY(char,_func_name,YELLOW,'d','i','s','a','s','m','_','j','u','m','p','s','\n',NONE,0)
#  define _ERR_T_DISASM_INSTRUCTION         PD_ARRAY(char,_func_name,YELLOW,'d','i','s','a','s','m','_','i','n','s','t','r','u','c','t','i','o','n','\n',NONE,0)
#  define _ERR_T_DISASM_OPERANDS            PD_ARRAY(char,_func_name,YELLOW,'d','i','s','a','s','m','_','o','p','e','r','a','n','d','s','\n',NONE,0)
#  define _ERR_T_MODRM                      PD_ARRAY(char,_func_name,YELLOW,'m','o','d','r','m','\n',NONE,0)
#  define _ERR_T_DISASM_LENGTH              PD_ARRAY(char,_func_name,YELLOW,'d','i','s','a','s','m','_','l','e','n','g','t','h','\n',NONE,0)
#  define _ERR_T_KNOWN_INSTRUCTION          PD_ARRAY(char,_func_name,YELLOW,'k','n','o','w','n','_','i','n','s','t','r','u','c','t','i','o','n','\n',NONE,0)
#  define _ERR_T_STEP_INSTRUCTION           PD_ARRAY(char,_func_name,YELLOW,'s','t','e','p','_','i','n','s','t','r','u','c','t','i','o','n','\n',NONE,0)
#  define _ERR_T_FOREACH_PHDR               PD_ARRAY(char,_func_name,YELLOW,'f','o','r','e','a','c','h','_','p','h','d','r','\n',NONE,0)
#  define _ERR_T_FOREACH_SHDR               PD_ARRAY(char,_func_name,YELLOW,'f','o','r','e','a','c','h','_','s','h','d','r','\n',NONE,0)
#  define _ERR_T_FOREACH_SHDR_ENTRY         PD_ARRAY(char,_func_name,YELLOW,'f','o','r','e','a','c','h','_','s','h','d','r','_','e','n','t','r','y','\n',NONE,0)
#  define _ERR_T_FIND_ENTRY                 PD_ARRAY(char,_func_name,YELLOW,'f','i','n','d','_','e','n','t','r','y','\n',NONE,0)
#  define _ERR_T_FIND_PHDR                  PD_ARRAY(char,_func_name,YELLOW,'f','i','n','d','_','p','h','d','r','\n',NONE,0)
#  define _ERR_T_FIND_SHDR                  PD_ARRAY(char,_func_name,YELLOW,'f','i','n','d','_','s','h','d','r','\n',NONE,0)
#  define _ERR_T_SETUP_ENTRY                PD_ARRAY(char,_func_name,YELLOW,'s','e','t','u','p','_','e','n','t','r','y','\n',NONE,0)
#  define _ERR_T_GENERATE_SEED              PD_ARRAY(char,_func_name,YELLOW,'g','e','n','e','r','a','t','e','_','s','e','e','d','\n',NONE,0)
#  define _ERR_T_COMPUTE_ELF_HDR_HASH       PD_ARRAY(char,_func_name,YELLOW,'c','o','m','p','u','t','e','_','e','l','f','_','h','d','r','_','h','a','s','h','\n',NONE,0)
#  define _ERR_T_INFECT                     PD_ARRAY(char,_func_name,YELLOW,'i','n','f','e','c','t','\n',NONE,0)
#  define _ERR_T_IS_ELF64                   PD_ARRAY(char,_func_name,YELLOW,'i','s','_','e','l','f','6','4','\n',NONE,0)
#  define _ERR_T_INFECTION_ENGINE           PD_ARRAY(char,_func_name,YELLOW,'i','n','f','e','c','t','i','o','n','_','e','n','g','i','n','e','\n',NONE,0)
#  define _ERR_T_WRITE_JMP32                PD_ARRAY(char,_func_name,YELLOW,'w','r','i','t','e','_','j','m','p','3','2','\n',NONE,0)
#  define _ERR_T_WRITE_I32_VALUE            PD_ARRAY(char,_func_name,YELLOW,'w','r','i','t','e','_','i','3','2','_','v','a','l','u','e','\n',NONE,0)
#  define _ERR_T_GET_JMP32_DESTINATION      PD_ARRAY(char,_func_name,YELLOW,'g','e','t','_','j','m','p','3','2','_','d','e','s','t','i','n','a','t','i','o','n','\n',NONE,0)
#  define _ERR_T_METAMORPH_CLONE            PD_ARRAY(char,_func_name,YELLOW,'m','e','t','a','m','o','r','p','h','_','c','l','o','n','e','\n',NONE,0)
#  define _ERR_T_INIT_LOADER_SAFE           PD_ARRAY(char,_func_name,YELLOW,'i','n','i','t','_','l','o','a','d','e','r','_','s','a','f','e','\n',NONE,0)
#  define _ERR_T_INIT_VIRUS_SAFE            PD_ARRAY(char,_func_name,YELLOW,'i','n','i','t','_','v','i','r','u','s','_','s','a','f','e','\n',NONE,0)
#  define _ERR_T_INIT_VIRUS_BUFFER_SAFE     PD_ARRAY(char,_func_name,YELLOW,'i','n','i','t','_','v','i','r','u','s','_','b','u','f','f','e','r','_','s','a','f','e','\n',NONE,0)
#  define _ERR_T_GET_VIRUS_ADDRESS          PD_ARRAY(char,_func_name,YELLOW,'g','e','t','_','v','i','r','u','s','_','a','d','d','r','e','s','s','\n',NONE,0)
#  define _ERR_T_ADJUST_VIRUS_CALL          PD_ARRAY(char,_func_name,YELLOW,'a','d','j','u','s','t','_','v','i','r','u','s','_','c','a','l','l','\n',NONE,0)
#  define _ERR_T_COPY_VIRUS_BUFFER_TO_CLONE PD_ARRAY(char,_func_name,YELLOW,'c','o','p','y','_','v','i','r','u','s','_','b','u','f','f','e','r','_','t','o','_','c','l','o','n','e','\n',NONE,0)
#  define _ERR_T_NOT_INFECTED               PD_ARRAY(char,_func_name,YELLOW,'n','o','t','_','i','n','f','e','c','t','e','d','\n',NONE,0)
#  define _ERR_T_CHECK_CLIENT_JUMP          PD_ARRAY(char,_func_name,YELLOW,'c','h','e','c','k','_','c','l','i','e','n','t','_','j','u','m','p','\n',NONE,0)
#  define _ERR_T_CHECK_SIGNATURE            PD_ARRAY(char,_func_name,YELLOW,'c','h','e','c','k','_','s','i','g','n','a','t','u','r','e','\n',NONE,0)
#  define _ERR_T_PERMUTATE_BLOCKS           PD_ARRAY(char,_func_name,YELLOW,'p','e','r','m','u','t','a','t','e','_','b','l','o','c','k','s','\n',NONE,0)
#  define _ERR_T_SHARD_BLOCK                PD_ARRAY(char,_func_name,YELLOW,'s','h','a','r','d','_','b','l','o','c','k','\n',NONE,0)
#  define _ERR_T_SHIFT_ENTRY_POINT          PD_ARRAY(char,_func_name,YELLOW,'s','h','i','f','t','_','e','n','t','r','y','_','p','o','i','n','t','\n',NONE,0)
#  define _ERR_T_WRITE_PERMUTATED_CODE      PD_ARRAY(char,_func_name,YELLOW,'w','r','i','t','e','_','p','e','r','m','u','t','a','t','e','d','_','c','o','d','e','\n',NONE,0)
#  define _ERR_T_RECURSIVE_SPLIT_BLOCKS     PD_ARRAY(char,_func_name,YELLOW,'r','e','c','u','r','s','i','v','e','_','s','p','l','i','t','_','b','l','o','c','k','s','\n',NONE,0)
#  define _ERR_T_SPLIT_BLOCK                PD_ARRAY(char,_func_name,YELLOW,'s','p','l','i','t','_','b','l','o','c','k','\n',NONE,0)
#  define _ERR_T_SPLIT_REF                  PD_ARRAY(char,_func_name,YELLOW,'s','p','l','i','t','_','r','e','f','\n',NONE,0)
#  define _ERR_T_SIZE_SPLIT_AT_HALF_BLOCK   PD_ARRAY(char,_func_name,YELLOW,'s','i','z','e','_','s','p','l','i','t','_','a','t','_','h','a','l','f','_','b','l','o','c','k','\n',NONE,0)
#  define _ERR_T_ADJUST_JUMPS               PD_ARRAY(char,_func_name,YELLOW,'a','d','j','u','s','t','_','j','u','m','p','s','\n',NONE,0)
#  define _ERR_T_ADD_TRAILING_JUMP          PD_ARRAY(char,_func_name,YELLOW,'a','d','d','_','t','r','a','i','l','i','n','g','_','j','u','m','p','\n',NONE,0)
#  define _ERR_T_PERMUTATE_INSTRUCTIONS     PD_ARRAY(char,_func_name,YELLOW,'p','e','r','m','u','t','a','t','e','_','i','n','s','t','r','u','c','t','i','o','n','s','\n',NONE,0)
#  define _ERR_T_APPLY_MATCHES              PD_ARRAY(char,_func_name,YELLOW,'a','p','p','l','y','_','m','a','t','c','h','e','s','\n',NONE,0)
#  define _ERR_T_APPLY_MATCH                PD_ARRAY(char,_func_name,YELLOW,'a','p','p','l','y','_','m','a','t','c','h','\n',NONE,0)
#  define _ERR_T_EXTENDED_OPCODE            PD_ARRAY(char,_func_name,YELLOW,'e','x','t','e','n','d','e','d','_','o','p','c','o','d','e','\n',NONE,0)
#  define _ERR_T_SETUP_VIRUS_HEADER         PD_ARRAY(char,_func_name,YELLOW,'s','e','t','u','p','_','v','i','r','u','s','_','h','e','a','d','e','r','\n',NONE,0)

/* ------------------------------ errors usage ------------------------------ */

#  define _ERR_U_NOT_ELF                    PD_ARRAY(char,_err_text,YELLOW,'n','o','t',' ','e','l','f','\n',NONE,0)
#  define _ERR_U_NOT_FILE                   PD_ARRAY(char,_err_text,YELLOW,'n','o','t',' ','a',' ','f','i','l','e','\n',NONE,0)

/* ------------------------------- errors file ------------------------------ */

#  define _ERR_F_READ_CLONE                 PD_ARRAY(char,_err_text,YELLOW,'f','a','i','l','e','d',' ','t','o',' ','r','e','a','d',' ','c','l','o','n','e','\n',NONE,0)
#  define _ERR_F_READ_EHDR                  PD_ARRAY(char,_err_text,YELLOW,'f','a','i','l','e','d',' ','t','o',' ','r','e','a','d',' ','e','l','f',' ','h','e','a','d','e','r','\n',NONE,0)
#  define _ERR_F_READ_PHDR                  PD_ARRAY(char,_err_text,YELLOW,'f','a','i','l','e','d',' ','t','o',' ','r','e','a','d',' ','p','r','o','g','r','a','m',' ','h','e','a','d','e','r','\n',NONE,0)
#  define _ERR_F_READ_SHDR                  PD_ARRAY(char,_err_text,YELLOW,'f','a','i','l','e','d',' ','t','o',' ','r','e','a','d',' ','s','e','c','t','i','o','n',' ','h','e','a','d','e','r','\n',NONE,0)
#  define _ERR_F_READ_PAYLOAD               PD_ARRAY(char,_err_text,YELLOW,'f','a','i','l','e','d',' ','t','o',' ','r','e','a','d',' ','p','a','y','l','o','a','d','\n',NONE,0)
#  define _ERR_F_READ_SIGNATURE             PD_ARRAY(char,_err_text,YELLOW,'f','a','i','l','e','d',' ','t','o',' ','r','e','a','d',' ','s','i','g','n','a','t','u','r','e','\n',NONE,0)
#  define _ERR_F_READ_LOADER                PD_ARRAY(char,_err_text,YELLOW,'f','a','i','l','e','d',' ','t','o',' ','r','e','a','d',' ','l','o','a','d','e','r','\n',NONE,0)
#  define _ERR_F_READ_VIRUS                 PD_ARRAY(char,_err_text,YELLOW,'f','a','i','l','e','d',' ','t','o',' ','r','e','a','d',' ','v','i','r','u','s','\n',NONE,0)
#  define _ERR_F_READ_VIRCALL               PD_ARRAY(char,_err_text,YELLOW,'f','a','i','l','e','d',' ','t','o',' ','r','e','a','d',' ','v','i','r','u','s',' ','c','a','l','l','\n',NONE,0)
#  define _ERR_F_READ_BLOCK                 PD_ARRAY(char,_err_text,YELLOW,'f','a','i','l','e','d',' ','t','o',' ','r','e','a','d',' ','b','l','o','c','k','\n',NONE,0)
#  define _ERR_F_FILE_START                 PD_ARRAY(char,_err_text,YELLOW,'f','a','i','l','e','d',' ','t','o',' ','r','e','a','d',' ','f','i','l','e',' ','s','t','a','r','t','\n',NONE,0)
#  define _ERR_F_FILE_END                   PD_ARRAY(char,_err_text,YELLOW,'f','a','i','l','e','d',' ','t','o',' ','r','e','a','d',' ','f','i','l','e',' ','e','n','d','\n',NONE,0)
#  define _ERR_F_CLONE_START                PD_ARRAY(char,_err_text,YELLOW,'f','a','i','l','e','d',' ','t','o',' ','r','e','a','d',' ','c','l','o','n','e',' ','s','t','a','r','t','\n',NONE,0)
#  define _ERR_F_CLONE_END                  PD_ARRAY(char,_err_text,YELLOW,'f','a','i','l','e','d',' ','t','o',' ','r','e','a','d',' ','c','l','o','n','e',' ','e','n','d','\n',NONE,0)
#  define _ERR_F_READ_CONSTANTS             PD_ARRAY(char,_err_text,YELLOW,'f','a','i','l','e','d',' ','t','o',' ','r','e','a','d',' ','c','o','n','s','t','a','n','t','s','\n',NONE,0)
#  define _ERR_F_READ_JUMP                  PD_ARRAY(char,_err_text,YELLOW,'f','a','i','l','e','d',' ','t','o',' ','r','e','a','d',' ','j','u','m','p','\n',NONE,0)
#  define _ERR_F_READ_RELA                  PD_ARRAY(char,_err_text,YELLOW,'f','a','i','l','e','d',' ','t','o',' ','r','e','a','d',' ','r','e','l','a','\n',NONE,0)
#  define _ERR_F_READ_REL                   PD_ARRAY(char,_err_text,YELLOW,'f','a','i','l','e','d',' ','t','o',' ','r','e','a','d',' ','r','e','l','\n',NONE,0)
#  define _ERR_F_READ_DYNSYM                PD_ARRAY(char,_err_text,YELLOW,'f','a','i','l','e','d',' ','t','o',' ','r','e','a','d',' ','d','y','n','s','y','m','\n',NONE,0)
#  define _ERR_F_READ_DYNAMIC               PD_ARRAY(char,_err_text,YELLOW,'f','a','i','l','e','d',' ','t','o',' ','r','e','a','d',' ','d','y','n','a','m','i','c','\n',NONE,0)
#  define _ERR_F_MANDATORY_FIELDS           PD_ARRAY(char,_err_text,YELLOW,'m','i','s','s','i','n','g',' ','m','a','n','d','a','t','o','r','y',' ','f','i','e','l','d','s','\n',NONE,0)
#  define _ERR_F_NOT_PIE                    PD_ARRAY(char,_err_text,YELLOW,'t','a','r','g','e','t',' ','i','s',' ','n','o','t',' ','p','o','s','i','t','i','o','n',' ','i','n','d','e','p','e','n','d','e','n','t','\n',NONE,0)

/* ----------------------------- errors system ------------------------------ */

#  define _ERR_S_OPEN                       PD_ARRAY(char,_err_text,YELLOW,'o','p','e','n',' ','f','a','i','l','e','d','\n',NONE,0)
#  define _ERR_S_CLOSE                      PD_ARRAY(char,_err_text,YELLOW,'c','l','o','s','e',' ','f','a','i','l','e','d','\n',NONE,0)
#  define _ERR_S_MMAP                       PD_ARRAY(char,_err_text,YELLOW,'m','m','a','p',' ','f','a','i','l','e','d','\n',NONE,0)
#  define _ERR_S_MUNMAP                     PD_ARRAY(char,_err_text,YELLOW,'m','u','n','m','a','p',' ','f','a','i','l','e','d','\n',NONE,0)
#  define _ERR_S_FSTAT                      PD_ARRAY(char,_err_text,YELLOW,'f','s','t','a','t',' ','f','a','i','l','e','d','\n',NONE,0)

/* ------------------------------ errors virus ------------------------------ */

#  define _ERR_V_ALREADY_INFECTED           PD_ARRAY(char,_err_text,GREEN,'a','l','r','e','a','d','y',' ','i','n','f','e','c','t','e','d',' ','<','3','\n',NONE,0)
#  define _ERR_V_MAX_JUMP_REACHED           PD_ARRAY(char,_err_text,YELLOW,'m','a','x','i','m','u','m',' ','j','u','m','p','s',' ','r','e','a','c','h','e','d','\n',NONE,0)
#  define _ERR_V_CANT_SPLIT_MORE            PD_ARRAY(char,_err_text,YELLOW,'c','a','n','\'','t',' ','s','p','l','i','t',' ','m','o','r','e','\n',NONE,0)
#  define _ERR_V_ENTRY_POINT                PD_ARRAY(char,_err_text,YELLOW,'c','o','u','l','d','n','\'','t',' ','f','i','n','d',' ','e','n','t','r','y',' ','p','o','i','n','t','\n',NONE,0)
#  define _ERR_V_READ_TRAILING_JUMP         PD_ARRAY(char,_err_text,YELLOW,'f','a','i','l','e','d',' ','t','o',' ','r','e','a','d',' ','t','r','a','i','l','i','n','g',' ','j','u','m','p','\n',NONE,0)
#  define _ERR_V_BAD_TRAILING_JUMP          PD_ARRAY(char,_err_text,YELLOW,'t','r','a','i','l','i','n','g',' ','j','u','m','p',' ','v','a','l','u','e',' ','d','o','e','s','n','\'','t',' ','m','a','t','c','h',' ','d','e','s','t','i','n','a','t','i','o','n','\n',NONE,0)
#  define _ERR_V_NOT_NEIGHBORS              PD_ARRAY(char,_err_text,YELLOW,'i','n','s','t','r','u','c','t','i','o','n','s',' ','a','r','e',' ','n','o','t',' ','n','e','i','g','h','b','o','u','r','s','\n',NONE,0)
#  define _ERR_V_INSTRUCTION_LENGTH         PD_ARRAY(char,_err_text,YELLOW,'i','n','s','t','r','u','c','t','i','o','n',' ','l','e','n','g','t','h',' ','e','x','c','e','e','d','s',' ','c','o','d','e',' ','l','e','n','g','t','h','\n',NONE,0)

/* -------------------------------------------------------------------------- */

static bool	_errors(const char *err_type, const char *err_text, const char *func_name)
{
	PD_ARRAY(char,err_throw,' ',' ','-','>',' ','i','n',' ','_',0);

	if (memcmp(err_text, (char[1]){0}, 1))
	{
		putstr(err_type); putstr(err_text);
	}
	putstr(err_throw); putstr(func_name);
	return false;
}

#  define errors(err_type, err_text, func_name)	({   \
	err_type;                                    \
	err_text;                                    \
	func_name;                                   \
	(_errors(_err_type, _err_text, _func_name)); \
})

# else
#  define errors(...)		false
# endif

#endif
