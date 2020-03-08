
#ifndef VISUAL_H
# define VISUAL_H

# include "position_independent.h"
# include "utils.h"

# ifdef DEBUG
#  define C_RED		'\033','[','3','1','m'
#  define C_GREEN	'\033','[','3','2','m'
#  define C_YELLOW	'\033','[','3','3','m'
#  define C_BLUE	'\033','[','3','4','m'
#  define C_MAGENTA	'\033','[','3','5','m'
#  define C_NONE	'\033','[','0','m'

#  define REGROUP(s1, n, s2)	({ \
	putstr(s1);                \
	putu64((uint64_t)n);       \
	putstr(s2);                \
})

#  define _VISUAL_NTH_BLOCK(nblock)	({                                                                                                                    \
	PD_ARRAY(char,vnb_s1,C_MAGENTA,'[','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-',' ','b','l','o','c','k',' ',0) \
	PD_ARRAY(char,vnb_s2,' ','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-',']','\n',C_NONE,0)                       \
	REGROUP(vnb_s1,nblock,vnb_s2);                                                                                                                        \
})

#  define _VISUAL_JUMPS(jumps)		({                                                                                                                    \
	PD_ARRAY(char,vcb_jumps_addr_s1,C_GREEN,'l','o','c','a','t','i','o','n',C_NONE,':',' ',C_RED,0)                                                       \
	PD_ARRAY(char,vcb_jumps_addr_s2,'\n',C_NONE,0)                                                                                                        \
	REGROUP(vcb_jumps_addr_s1,p_jumps->location,vcb_jumps_addr_s2);                                                                                       \
})

#  define print_jumps(jumps, njumps)		({                                                                                                                               \
	PD_ARRAY(char,vcb_jumps,C_BLUE,' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','j','u','m','p','s','\n',0);              \
	putstr(vcb_jumps);                                                                                                                                                       \
	struct jump	*p_jumps = jumps;                                                                                                                                        \
	for (size_t n = 0; n < njumps && p_jumps; n++)                                                                                                                           \
	{                                                                                                                                                                        \
		_VISUAL_JUMPS(p_jumps);                                                                                                                                          \
		p_jumps++;                                                                                                                                                       \
	}                                                                                                                                                                        \
})

#  define _VISUAL_LABELS(label)		({ \
	PD_ARRAY(char,vcb_labels_addr_s1,C_GREEN,'l','o','c','a','t','i','o','n',C_NONE,':',' ',C_RED,0)                                                      \
	PD_ARRAY(char,vcb_labels_addr_s2,' ',C_NONE,0)                                                                                                        \
	REGROUP(vcb_labels_addr_s1,p_labels->location,vcb_labels_addr_s2);                                                                                    \
	PD_ARRAY(char,vcb_labels_jumps_s1,C_GREEN,'j','u','m','p','s',C_NONE,':',' ',C_RED,0)                                                                 \
	PD_ARRAY(char,vcb_labels_jumps_s2,' ',C_NONE,0)                                                                                                       \
	REGROUP(vcb_labels_jumps_s1,p_labels->jumps,vcb_labels_jumps_s2);                                                                                     \
	PD_ARRAY(char,vcb_labels_njumps_s1,C_GREEN,'n','j','u','m','p','s',C_NONE,':',' ',C_RED,0)                                                            \
	PD_ARRAY(char,vcb_labels_njumps_s2,'\n',C_NONE,0)                                                                                                     \
	REGROUP(vcb_labels_njumps_s1,p_labels->njumps,vcb_labels_njumps_s2);                                                                                  \
})

#  define print_labels(labels, nlabels)		({ \
	PD_ARRAY(char,vcb_labels,C_BLUE,' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','l','a','b','e','l','s','\n',C_NONE,0); \
	putstr(vcb_labels); \
	struct label	*p_labels = labels; \
	for (size_t n = 0; n < nlabels && p_labels; n++)                                                                                                                         \
	{                                                                                                                                                                        \
		_VISUAL_LABELS(p_labels);                                                                                                                                        \
		p_labels++;                                                                                                                                                      \
	}                                                                                                                                                                        \
})

#  define _VISUAL_CODE_BLOCK(block)	({                                                                                                                                       \
	PD_ARRAY(char,vcb,C_BLUE,' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','c','o','d','e',' ','b','l','o','c','k','\n',C_NONE,0); \
	putstr(vcb);                                                                                                                                                             \
	PD_ARRAY(char,vcb_addr_s1,C_GREEN,'a','d','d','r','e','s','s',C_NONE,':',' ',C_RED,0)                                                                                    \
	PD_ARRAY(char,vcb_addr_s2,'\n',C_NONE,0)                                                                                                                                 \
	REGROUP(vcb_addr_s1,block->ref.ptr,vcb_addr_s2);                                                                                                                         \
	PD_ARRAY(char,vcb_size_s1,C_GREEN,'s','i','z','e',' ',' ',' ',C_NONE,':',' ',C_RED,0)                                                                                    \
	PD_ARRAY(char,vcb_size_s2,'\n',C_NONE,0)                                                                                                                                 \
	REGROUP(vcb_size_s1,block->ref.size,vcb_size_s2);                                                                                                                        \
	PD_ARRAY(char,vcb_njumps_s1,C_GREEN,'n','j','u','m','p','s',' ',C_NONE,':',' ',C_RED,0)                                                                                  \
	PD_ARRAY(char,vcb_njumps_s2,'\n',C_NONE,0)                                                                                                                               \
	REGROUP(vcb_njumps_s1,block->njumps,vcb_njumps_s2);                                                                                                                      \
	PD_ARRAY(char,vcb_nlabels_s1,C_GREEN,'n','l','a','b','e','l','s',C_NONE,':',' ',C_RED,0)                                                                                 \
	PD_ARRAY(char,vcb_nlabels_s2,'\n',C_NONE,0)                                                                                                                              \
	REGROUP(vcb_nlabels_s1,block->nlabels,vcb_nlabels_s2);                                                                                                                   \
})

#  define print_code_blocks(blocks, nblocks)	({                                                                                                                               \
	struct code_block	*p_blocks = blocks;                                                                                                                              \
	for (size_t nb = 0; nb < MAX_BLOCKS && p_blocks; nb++)                                                                                                                   \
	{                                                                                                                                                                        \
		if (p_blocks->ref.size == 0) break;                                                                                                                              \
		_VISUAL_NTH_BLOCK(nb);                                                                                                                                           \
		_VISUAL_CODE_BLOCK(p_blocks);                                                                                                                                    \
		print_jumps(p_blocks->jumps, p_blocks->njumps);                                                                                                                  \
		print_labels(p_blocks->labels, p_blocks->nlabels);                                                                                                               \
		p_blocks++;                                                                                                                                                      \
	}                                                                                                                                                                        \
})
#  define print_block_allocation(blocks)	({                                                                                                                               \
	struct block_allocation	*p_block_allocation = blocks;                                                                                                                    \
	print_code_blocks(p_block_allocation->blocks, MAX_BLOCKS);                                                                                                               \
	print_jumps(p_block_allocation->jumps, MAX_JUMPS);                                                                                                                       \
	print_labels(p_block_allocation->labels, MAX_JUMPS);                                                                                                                     \
})

# else
#  define visual(...)
#  define print_code_blocks(...)
#  define print_block_allocation(...)
# endif

#endif
