
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

#  define REGROUPU64(s1, n, s2)	({                                             \
	putstr(s1);                                                            \
	putu64((uint64_t)n);                                                   \
	putstr(s2);                                                            \
})

#  define REGROUP32(s1, n, s2)	({                                             \
	putstr(s1);                                                            \
	dput32((int32_t)n);                                                    \
	putstr(s2);                                                            \
})

#  define _VISUAL_NTH_BLOCK(nblock)	({                                                                                                                                                                        \
	PD_ARRAY(char,vnb_s1,C_MAGENTA,'[','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-',' ','b','l','o','c','k',' ',0) \
	PD_ARRAY(char,vnb_s2,' ','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-',']','\n','\n',C_NONE,0)              \
	REGROUPU64(vnb_s1,nblock,vnb_s2);                                                                                                                                                                         \
	putstr(nl);                                                                                                                                                                                               \
})

#  define print_head(n) ({                                                     \
	_VISUAL_NTH_BLOCK(n);                                                  \
})

#  define _VISUAL_LABELS(label)		({ \
	PD_ARRAY(char,vcb_labels_addr,C_GREEN,'l','o','c','a','t','i','o','n',C_NONE,':',' ',C_RED,0) \
	PD_ARRAY(char,vcb_labels_jumps,C_GREEN,'j','u','m','p','s',C_NONE,':',' ',C_RED,0)            \
	PD_ARRAY(char,vcb_labels_njumps,C_GREEN,'n','j','u','m','p','s',C_NONE,':',' ',C_RED,0)       \
	REGROUPU64(vcb_labels_addr,p_labels->location,sp);                                            \
	REGROUP32(vcb_labels_jumps,p_labels->jumps,sp);                                               \
	REGROUP32(vcb_labels_njumps,p_labels->njumps,nl);                                             \
})

#  define print_labels(labels, nlabels)		({                                                                                                                                                                                       \
	PD_ARRAY(char,vcb_labels,C_BLUE,' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','l','a','b','e','l','s','\n','\n',C_NONE,0); \
	putstr(vcb_labels);                                                                                                                                                                                                              \
	struct label	*p_labels = labels;                                                                                                                                                                                              \
	for (size_t n = 0; n < nlabels && p_labels; n++)                                                                                                                                                                                 \
	{                                                                                                                                                                                                                                \
		_VISUAL_LABELS(p_labels);                                                                                                                                                                                                \
		p_labels++;                                                                                                                                                                                                              \
	}                                                                                                                                                                                                                                \
	putstr(nl);                                                                                                                                                                                                                      \
})

#  define _VISUAL_JUMPS(jump)		({                                                                                  \
	struct control_flow	cf[1];                                                                                      \
	disasm_jumps(cf, 1, jump->location, 16);                                                                            \
	PD_ARRAY(char,vcb_jump_addr,C_GREEN,'l','o','c','a','t','i','o','n',C_NONE,':',' ',C_RED,0)                         \
	PD_ARRAY(char,vcb_jump_target,C_GREEN,'t','a','r','g','e','t',C_NONE,':',' ',C_RED,0)                               \
	PD_ARRAY(char,vcb_jump_displacement,C_GREEN,'d','i','s','p','l','a','c','e','m','e','n','t',C_NONE,':',' ',C_RED,0) \
	REGROUPU64(vcb_jump_addr,jump->location,sp);                                                                        \
	REGROUPU64(vcb_jump_target,cf->label_addr,sp);                                                                      \
	REGROUP32(vcb_jump_displacement,cf->value,nl);                                                                      \
})

#  define print_jumps(jumps, njumps)		({                                                                                                                                                                           \
	PD_ARRAY(char,vcb_jumps,C_BLUE,' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','j','u','m','p','s','\n','\n',0); \
	putstr(vcb_jumps);                                                                                                                                                                                                   \
	struct jump	*p_jumps = jumps;                                                                                                                                                                                    \
	for (size_t n = 0; n < njumps && p_jumps; n++)                                                                                                                                                                       \
	{                                                                                                                                                                                                                    \
		_VISUAL_JUMPS(p_jumps);                                                                                                                                                                                      \
		p_jumps++;                                                                                                                                                                                                   \
	}                                                                                                                                                                                                                    \
	putstr(nl);                                                                                                                                                                                                          \
})

#  define _VISUAL_CODE_BLOCK(block)	({                                                                                                                                                                                           \
	PD_ARRAY(char,vcb,C_BLUE,' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','c','o','d','e',' ','b','l','o','c','k','\n',C_NONE,0); \
	PD_ARRAY(char,vcb_addr,C_GREEN,'a','d','d','r','e','s','s',C_NONE,':',' ',C_RED,0)                                                                                                                                           \
	PD_ARRAY(char,vcb_size,C_GREEN,'s','i','z','e',' ',' ',' ',C_NONE,':',' ',C_RED,0)                                                                                                                                           \
	PD_ARRAY(char,vcb_njumps,C_GREEN,'n','j','u','m','p','s',' ',C_NONE,':',' ',C_RED,0)                                                                                                                                         \
	PD_ARRAY(char,vcb_nlabels,C_GREEN,'n','l','a','b','e','l','s',C_NONE,':',' ',C_RED,0)                                                                                                                                        \
	putstr(vcb);                                                                                                                                                                                                                 \
	REGROUPU64(vcb_addr,block->ref.ptr,nl);                                                                                                                                                                                      \
	REGROUP32(vcb_size,block->ref.size,nl);                                                                                                                                                                                      \
	REGROUP32(vcb_njumps,block->njumps,nl);                                                                                                                                                                                      \
	REGROUP32(vcb_nlabels,block->nlabels,nl);                                                                                                                                                                                    \
})

#  define print_code_blocks(blocks, nblocks)	({                             \
	PD_ARRAY(char,nl,'\n',0);                                              \
	PD_ARRAY(char,sp,' ',0);                                               \
	struct code_block	*p_blocks = blocks;                            \
	for (size_t nb = 0; nb < MAX_BLOCKS && p_blocks; nb++)                 \
	{                                                                      \
		if (p_blocks->ref.size == 0) break;                            \
		print_head(nb);                                                \
		_VISUAL_CODE_BLOCK(p_blocks);                                  \
		print_jumps(p_blocks->jumps, p_blocks->njumps);                \
		print_labels(p_blocks->labels, p_blocks->nlabels);             \
		p_blocks++;                                                    \
	}                                                                      \
})

# else
#  define visual(...)
#  define print_jumps(...)
#  define print_labels(...)
#  define print_code_blocks(...)
# endif

#endif
