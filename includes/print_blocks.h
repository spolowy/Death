
#ifndef PRINT_BLOCKS_H
# define PRINT_BLOCKS_H

# ifdef DEBUG

# include "position_independent.h"
# include "utils.h"

/* --------------------------------- Colors --------------------------------- */

#  define C_RED		'\033','[','0',';','3','1','m'
#  define C_GREEN	'\033','[','0',';','3','2','m'
#  define C_YELLOW	'\033','[','0',';','3','3','m'
#  define C_BLUE	'\033','[','0',';','3','4','m'
#  define C_MAGENTA	'\033','[','0',';','3','5','m'
#  define C_CYAN	'\033','[','0',';','3','6','m'
#  define C_GREY	'\033','[','1',';','3','0','m'
#  define C_NONE	'\033','[','0','m'

/* -------------------------------------------------------------------------- */
/* ---------------------------------- Utils --------------------------------- */
/* -------------------------------------------------------------------------- */

/* -------------------------------- Regroups -------------------------------- */

#  define REGROUPU64(s1, n, s2) ({                                             \
	putstr(s1);                                                            \
	putu64((uint64_t)n);                                                   \
	putstr(s2);                                                            \
})

#  define REGROUP32(s1, n, s2) ({                                              \
	putstr(s1);                                                            \
	dput32((int32_t)n);                                                    \
	putstr(s2);                                                            \
})

#  define IN_OUT(value, input_code, output_buffer, shift_amount) (                                               \
	(size_t)((ssize_t)(((value) - ((size_t)input_code.ptr)) + ((size_t)output_buffer.ptr)) + (shift_amount)) \
)

#  define REGROUPU64_ARROW(s1, n1, s2, s3, n2, s4) ({                          \
	putstr(s1);                                                            \
	putu64((uint64_t)n1);                                                  \
	putstr(s2);                                                            \
	putstr(s3);                                                            \
	putu64((uint64_t)n2);                                                  \
	putstr(s4);                                                            \
})

/* -------------------------------------------------------------------------- */
/* -------------------------- Print Original Block -------------------------- */
/* -------------------------------------------------------------------------- */

/* --------------------------------- Labels --------------------------------- */

#  define _ORIGINAL_LABELS(label) ({                                                                    \
	PD_ARRAY(char,vcb_labels_addr,C_GREEN,'a','d','d','r','e','s','s',C_NONE,':',' ',C_RED,0)       \
	PD_ARRAY(char,vcb_labels_jumps,C_GREEN,'j','u','m','p','s',C_NONE,':',' ',C_RED,0)              \
	PD_ARRAY(char,vcb_labels_njumps,C_GREEN,'n','j','u','m','p','s',C_NONE,':',' ',C_RED,0)         \
	PD_ARRAY(char,vcb_labels_jump,C_RED,0)                                                          \
	PD_ARRAY(char,sep,C_GREY,'/',0)                                                                 \
	REGROUPU64(vcb_labels_addr,label->location,sp);                                                 \
	REGROUP32(vcb_labels_njumps,label->njumps,sp);                                                  \
	putstr(vcb_labels_jumps);                                                                       \
	struct jump	**p_jumps = label->jumps;                                                       \
	for (size_t n = 0; n < label->njumps; n++)                                                      \
	{                                                                                               \
		struct jump *p_jump = (struct jump*)(p_jumps[n]);                                       \
		REGROUPU64(vcb_labels_jump,p_jump->location,"");                                        \
		if (n + 1 < label->njumps && p_jump + 1 != NULL)                                        \
			putstr(sep);                                                                    \
	}                                                                                               \
	putstr(nl);                                                                                     \
})

#  define _print_original_labels(labels, nlabels) ({                                                                                                                                                                                     \
	PD_ARRAY(char,vcb_labels,C_BLUE,' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','l','a','b','e','l','s','\n','\n',C_NONE,0); \
	putstr(vcb_labels);                                                                                                                                                                                                              \
	struct label	*p_labels = labels;                                                                                                                                                                                              \
	for (size_t n = 0; n < nlabels && p_labels; n++)                                                                                                                                                                                 \
	{                                                                                                                                                                                                                                \
		_ORIGINAL_LABELS(p_labels);                                                                                                                                                                                              \
		p_labels++;                                                                                                                                                                                                              \
	}                                                                                                                                                                                                                                \
	putstr(nl);                                                                                                                                                                                                                      \
})

/* --------------------------------- Jumps ---------------------------------- */

#  define _ORIGINAL_JUMPS(jump) ({                                                                                                       \
	PD_ARRAY(char,vcb_jump_addr,C_GREEN,'a','d','d','r','e','s','s',C_NONE,':',' ',C_RED,0)                                          \
	PD_ARRAY(char,vcb_jump_target,C_GREEN,'s','i','z','e',C_NONE,':',' ',C_RED,0)                                                    \
	PD_ARRAY(char,vcb_jump_value_addr,C_GREEN,'v','a','l','u','e','_','a','d','d','r','e','s','s',C_NONE,':',' ',C_RED,0)            \
	PD_ARRAY(char,vcb_jump_value_size,C_GREEN,'v','a','l','u','e','_','s','i','z','e',C_NONE,':',' ',C_RED,0)                        \
	PD_ARRAY(char,vcb_jump_displacement,C_GREEN,'v','a','l','u','e',C_NONE,':',' ',C_RED,0)                                          \
	PD_ARRAY(char,vcb_jump_shift_amount,C_GREEN,'s','h','i','f','t','_','a','m','o','u','n','t',C_NONE,':',' ',C_RED,0)              \
	REGROUPU64(vcb_jump_addr,jump->location,sp);                                                                                     \
	REGROUP32(vcb_jump_target,jump->instruction_size,sp);                                                                            \
	REGROUPU64(vcb_jump_value_addr,jump->value_addr,sp);                                                                             \
	REGROUP32(vcb_jump_value_size,jump->value_size,sp);                                                                              \
	REGROUP32(vcb_jump_displacement,jump->value,sp);                                                                                 \
	REGROUP32(vcb_jump_shift_amount,jump->value_shift,nl);                                                                           \
})

#  define _print_original_jumps(jumps, njumps) ({                                                                                                                                                                            \
	PD_ARRAY(char,vcb_jumps,C_BLUE,' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','j','u','m','p','s','\n','\n',0); \
	putstr(vcb_jumps);                                                                                                                                                                                                   \
	struct jump	*p_jumps = jumps;                                                                                                                                                                                    \
	for (size_t n = 0; n < njumps && p_jumps; n++)                                                                                                                                                                       \
	{                                                                                                                                                                                                                    \
		_ORIGINAL_JUMPS(p_jumps);                                                                                                                                                                                    \
		p_jumps++;                                                                                                                                                                                                   \
	}                                                                                                                                                                                                                    \
	putstr(nl);                                                                                                                                                                                                          \
})

/* ------------------------------- Code Block ------------------------------- */

#  define _ORIGINAL_CODE_BLOCK(block) ({                                                                                                                                                                                             \
	PD_ARRAY(char,vcb,C_BLUE,' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','c','o','d','e',' ','b','l','o','c','k','\n',C_NONE,0); \
	PD_ARRAY(char,vcb_addr,C_GREEN,'a','d','d','r','e','s','s',C_NONE,':',' ',C_RED,0)                                                                                                                                           \
	PD_ARRAY(char,vcb_size,C_GREEN,'s','i','z','e',' ',' ',' ',C_NONE,':',' ',C_RED,0)                                                                                                                                           \
	PD_ARRAY(char,vcb_njumps,C_GREEN,'n','j','u','m','p','s',' ',C_NONE,':',' ',C_RED,0)                                                                                                                                         \
	PD_ARRAY(char,vcb_nlabels,C_GREEN,'n','l','a','b','e','l','s',C_NONE,':',' ',C_RED,0)                                                                                                                                        \
	PD_ARRAY(char,vcb_shift_amount,C_GREEN,'s','h','i','f','t','_','a','m','o','u','n','t',C_NONE,':',' ',C_RED,0)                                                                                                               \
	putstr(vcb);                                                                                                                                                                                                                 \
	REGROUPU64(vcb_addr,block->ref.ptr,nl);                                                                                                                                                                                      \
	REGROUP32(vcb_size,block->ref.size,nl);                                                                                                                                                                                      \
	REGROUP32(vcb_njumps,block->njumps,nl);                                                                                                                                                                                      \
	REGROUP32(vcb_nlabels,block->nlabels,nl);                                                                                                                                                                                    \
	REGROUP32(vcb_shift_amount,block->shift_amount,nl);                                                                                                                                                                          \
})

#  define _print_original_block(block) ({                                      \
	PD_ARRAY(char,nl,'\n',0);                                              \
	PD_ARRAY(char,sp,' ',0);                                               \
	_ORIGINAL_CODE_BLOCK(block);                                           \
	_print_original_jumps(block->jumps, block->njumps);                    \
	_print_original_labels(block->labels, block->nlabels);                 \
})

#  define debug_print_original_block(blocks) ({                                                          \
	PD_ARRAY(char,nl,'\n',0);                                                                        \
	PD_ARRAY(char,c_cyan,C_CYAN,0)                                                                   \
	PD_ARRAY(char,c_none,C_NONE,0)                                                                   \
	PD_ARRAY(char,original_block,'#',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','#','\n','#',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','O','r','i','g','i','n','a','l',' ','B','l','o','c','k',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','#','\n','#',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','#','\n','\n',0);                                                          \
	putstr(c_cyan); putstr(nl); putstr(nl);                                                          \
	putstr(original_block);                                                                          \
	_print_original_block(blocks);                                                                   \
	putstr(c_none);                                                                                  \
})

/* -------------------------------------------------------------------------- */
/* --------------------------- Print Split Blocks --------------------------- */
/* -------------------------------------------------------------------------- */

/* --------------------------------- Header --------------------------------- */

#  define _NTH_BLOCK(nblock) ({                                                                                                                                                                                   \
	PD_ARRAY(char,vnb_s1,C_MAGENTA,'[','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-',' ','b','l','o','c','k',' ',0) \
	PD_ARRAY(char,vnb_s2,' ','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-',']','\n','\n',C_NONE,0)              \
	REGROUP32(vnb_s1,nblock,vnb_s2);                                                                                                                                                                          \
	putstr(nl);                                                                                                                                                                                               \
})

#  define _print_head(n) ({                                                    \
	_NTH_BLOCK(n);                                                         \
})

/* --------------------------------- Labels --------------------------------- */

#  define _SPLIT_LABELS(label, input_code, output_buffer) ({                                                                                                         \
	PD_ARRAY(char,vcb_labels_addr,C_GREEN,'a','d','d','r','e','s','s',C_NONE,':',' ',C_RED,0)                                                                    \
	PD_ARRAY(char,vcb_labels_jumps,C_GREEN,'j','u','m','p','s',C_NONE,':',' ',C_RED,0)                                                                           \
	PD_ARRAY(char,vcb_labels_njumps,C_GREEN,'n','j','u','m','p','s',C_NONE,':',' ',C_RED,0)                                                                      \
	PD_ARRAY(char,vcb_labels_jump,C_RED,0)                                                                                                                       \
	PD_ARRAY(char,sep,C_GREY,'/',0)                                                                                                                              \
	PD_ARRAY(char,vcb_arrow,C_GREY,'-','>',0)                                                                                                                    \
	PD_ARRAY(char,vcb_red,C_RED,0)                                                                                                                               \
	REGROUPU64_ARROW(vcb_labels_addr,label->location,vcb_arrow,vcb_red,IN_OUT(label->location,input_code,output_buffer,p_blocks->shift_amount),sp);              \
	REGROUP32(vcb_labels_njumps,label->njumps,sp);                                                                                                               \
	putstr(vcb_labels_jumps);                                                                                                                                    \
	struct jump	**p_jumps = label->jumps;                                                                                                                    \
	for (size_t n = 0; n < label->njumps; n++)                                                                                                                   \
	{                                                                                                                                                            \
		struct jump *p_jump = (struct jump*)(p_jumps[n]);                                                                                                    \
		REGROUPU64_ARROW(vcb_labels_jump,p_jump->location,vcb_arrow,vcb_red,IN_OUT(p_jump->location,input_code,output_buffer,p_blocks->shift_amount),"");    \
		if (n + 1 < label->njumps && p_jump + 1 != NULL)                                                                                                     \
			putstr(sep);                                                                                                                                 \
	}                                                                                                                                                            \
	putstr(nl);                                                                                                                                                  \
})

#  define _print_split_labels(labels, nlabels, input_code, output_buffer) ({                                                                                                                                                             \
	PD_ARRAY(char,vcb_labels,C_BLUE,' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','l','a','b','e','l','s','\n','\n',C_NONE,0); \
	putstr(vcb_labels);                                                                                                                                                                                                              \
	struct label	*p_labels = labels;                                                                                                                                                                                              \
	for (size_t n = 0; n < nlabels && p_labels; n++)                                                                                                                                                                                 \
	{                                                                                                                                                                                                                                \
		_SPLIT_LABELS(p_labels, input_code, output_buffer);                                                                                                                                                                      \
		p_labels++;                                                                                                                                                                                                              \
	}                                                                                                                                                                                                                                \
	putstr(nl);                                                                                                                                                                                                                      \
})

/* --------------------------------- Jumps ---------------------------------- */

#  define _SPLIT_JUMPS(jump, input_code, output_buffer) ({                                                                                                      \
	PD_ARRAY(char,vcb_jump_addr,C_GREEN,'a','d','d','r','e','s','s',C_NONE,':',' ',C_RED,0)                                                                 \
	PD_ARRAY(char,vcb_jump_target,C_GREEN,'s','i','z','e',C_NONE,':',' ',C_RED,0)                                                                           \
	PD_ARRAY(char,vcb_jump_value_addr,C_GREEN,'v','a','l','u','e','_','a','d','d','r','e','s','s',C_NONE,':',' ',C_RED,0)                                   \
	PD_ARRAY(char,vcb_jump_value_size,C_GREEN,'v','a','l','u','e','_','s','i','z','e',C_NONE,':',' ',C_RED,0)                                               \
	PD_ARRAY(char,vcb_jump_displacement,C_GREEN,'v','a','l','u','e',C_NONE,':',' ',C_RED,0)                                                                 \
	PD_ARRAY(char,vcb_jump_shift_amount,C_GREEN,'s','h','i','f','t','_','a','m','o','u','n','t',C_NONE,':',' ',C_RED,0)                                     \
	PD_ARRAY(char,vcb_arrow,C_GREY,'-','>',0)                                                                                                               \
	PD_ARRAY(char,vcb_red,C_RED,0)                                                                                                                          \
	REGROUPU64_ARROW(vcb_jump_addr,jump->location,vcb_arrow,vcb_red,IN_OUT(jump->location,input_code,output_buffer,p_blocks->shift_amount),sp);             \
	REGROUP32(vcb_jump_target,jump->instruction_size,sp);                                                                                                   \
	REGROUPU64_ARROW(vcb_jump_value_addr,jump->value_addr,vcb_arrow,vcb_red,IN_OUT(jump->value_addr,input_code,output_buffer,p_blocks->shift_amount),sp);   \
	REGROUP32(vcb_jump_value_size,jump->value_size,sp);                                                                                                     \
	REGROUP32(vcb_jump_displacement,jump->value,sp);                                                                                                        \
	REGROUP32(vcb_jump_shift_amount,jump->value_shift,nl);                                                                                                  \
})

#  define _print_split_jumps(jumps, njumps, input_code, output_buffer) ({                                                                                                                                                    \
	PD_ARRAY(char,vcb_jumps,C_BLUE,' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','j','u','m','p','s','\n','\n',0); \
	putstr(vcb_jumps);                                                                                                                                                                                                   \
	struct jump	*p_jumps = jumps;                                                                                                                                                                                    \
	for (size_t n = 0; n < njumps && p_jumps; n++)                                                                                                                                                                       \
	{                                                                                                                                                                                                                    \
		_SPLIT_JUMPS(p_jumps, input_code, output_buffer);                                                                                                                                                            \
		p_jumps++;                                                                                                                                                                                                   \
	}                                                                                                                                                                                                                    \
	putstr(nl);                                                                                                                                                                                                          \
})

/* ------------------------------- Code Block ------------------------------- */

#  define _SPLIT_CODE_BLOCK(block, input_code, output_buffer) ({                                                                                                                                                                     \
	PD_ARRAY(char,vcb,C_BLUE,' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','c','o','d','e',' ','b','l','o','c','k','\n',C_NONE,0); \
	PD_ARRAY(char,vcb_addr,C_GREEN,'a','d','d','r','e','s','s',C_NONE,':',' ',C_RED,0)                                                                                                                                           \
	PD_ARRAY(char,vcb_size,C_GREEN,'s','i','z','e',' ',' ',' ',C_NONE,':',' ',C_RED,0)                                                                                                                                           \
	PD_ARRAY(char,vcb_njumps,C_GREEN,'n','j','u','m','p','s',' ',C_NONE,':',' ',C_RED,0)                                                                                                                                         \
	PD_ARRAY(char,vcb_nlabels,C_GREEN,'n','l','a','b','e','l','s',C_NONE,':',' ',C_RED,0)                                                                                                                                        \
	PD_ARRAY(char,vcb_shift_amount,C_GREEN,'s','h','i','f','t','_','a','m','o','u','n','t',C_NONE,':',' ',C_RED,0)                                                                                                               \
	PD_ARRAY(char,vcb_trailing_block,C_GREEN,'t','r','a','i','l','i','n','g','_','b','l','o','c','k', C_NONE,':',' ',C_RED,0)                                                                                                    \
	PD_ARRAY(char,vcb_arrow,C_GREY,'-','>',0)                                                                                                                                                                                    \
	PD_ARRAY(char,vcb_red,C_RED,0)                                                                                                                                                                                               \
	putstr(vcb);                                                                                                                                                                                                                 \
	REGROUPU64_ARROW(vcb_addr,block->ref.ptr,vcb_arrow,vcb_red,IN_OUT(block->ref.ptr,input_code,output_buffer,p_blocks->shift_amount),nl);                                                                                       \
	REGROUP32(vcb_size,block->ref.size,nl);                                                                                                                                                                                      \
	REGROUP32(vcb_njumps,block->njumps,nl);                                                                                                                                                                                      \
	REGROUP32(vcb_nlabels,block->nlabels,nl);                                                                                                                                                                                    \
	REGROUP32(vcb_shift_amount,block->shift_amount,nl);                                                                                                                                                                          \
	if (block->trailing_block)                                                                                                                                                                                                   \
		REGROUPU64_ARROW(vcb_trailing_block,block->ref.ptr,vcb_arrow,vcb_red,IN_OUT(block->trailing_block->ref.ptr,input_code,output_buffer,block->trailing_block->shift_amount),nl);                                        \
	else                                                                                                                                                                                                                         \
		REGROUPU64(vcb_trailing_block,block->trailing_block,nl);                                                                                                                                                             \
})

#  define _print_split_blocks(blocks, nblocks, input_code, output_buffer) ({                         \
	PD_ARRAY(char,nl,'\n',0);                                                                    \
	PD_ARRAY(char,sp,' ',0);                                                                     \
	struct code_block	*p_blocks = blocks;                                                  \
	for (size_t nb = 0; nb < nblocks && p_blocks; nb++)                                          \
	{                                                                                            \
		_print_head(nb);                                                                     \
		_SPLIT_CODE_BLOCK(p_blocks, input_code, output_buffer);                              \
		_print_split_jumps(p_blocks->jumps, p_blocks->njumps, input_code, output_buffer);    \
		_print_split_labels(p_blocks->labels, p_blocks->nlabels, input_code, output_buffer); \
		p_blocks++;                                                                          \
	}                                                                                            \
})

/* ------------------------------- Code Block ------------------------------- */

#  define debug_print_split_blocks(blocks, nblocks, input_code, output_buffer) ({                        \
	PD_ARRAY(char,nl,'\n',0);                                                                        \
	PD_ARRAY(char,c_cyan,C_CYAN,0)                                                                   \
	PD_ARRAY(char,c_none,C_NONE,0)                                                                   \
	PD_ARRAY(char,split_blocks,'#',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','#','\n','#',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','S','p','l','i','t',' ','B','l','o','c','k','s',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','#','\n','#',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','#','\n','\n',0)                                                               \
	putstr(c_cyan); putstr(nl); putstr(nl);                                                          \
	putstr(split_blocks);                                                                            \
	_print_split_blocks(blocks, nblocks, input_code, output_buffer);                                 \
	putstr(c_none);                                                                                  \
})

/* -------------------------------------------------------------------------- */
/* --------------------------- Print General Info --------------------------- */
/* -------------------------------------------------------------------------- */

# define debug_print_general(input_code, output_buffer, entry_point, virus_func_shift, output_size, seed) ({                 \
	PD_ARRAY(char,nl,'\n',0);                                                                                            \
	PD_ARRAY(char,ia,C_YELLOW,'i','n','p','u','t','_','a','d','d','r', C_NONE,':',' ',C_RED,0)                           \
	PD_ARRAY(char,is,C_YELLOW,'i','n','p','u','t','_','s','i','z','e', C_NONE,':',' ',C_RED,0)                           \
	PD_ARRAY(char,ba,C_YELLOW,'b','u','f','f','e','r','_','a','d','d','r', C_NONE,':',' ',C_RED,0)                       \
	PD_ARRAY(char,bs,C_YELLOW,'b','u','f','f','e','r','_','s','i','z','e', C_NONE,':',' ',C_RED,0)                       \
	PD_ARRAY(char,ep,C_YELLOW,'e','n','t','r','y','_','p','o','i','n','t', C_NONE,':',' ',C_RED,0)                       \
	PD_ARRAY(char,fc,C_YELLOW,'v','i','r','u','s','_','f','u','n','c','_','s','h','i','f','t', C_NONE,':',' ',C_RED,0)   \
	PD_ARRAY(char,os,C_YELLOW,'o','u','t','p','u','t','_','s','i','z','e', C_NONE,':',' ',C_RED,0)                       \
	PD_ARRAY(char,sd,C_YELLOW,'s','e','e','d', C_NONE,':',' ',C_RED,0)                                                   \
	PD_ARRAY(char,c_cyan,C_CYAN,0)                                                                                       \
	PD_ARRAY(char,c_none,C_NONE,0)                                                                                       \
	PD_ARRAY(char,general_info,'#',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','#','\n','#',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','G','e','n','e','r','a','l',' ','I','n','f','o',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','#','\n','#',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','#','\n','\n',0);                                                                                  \
	putstr(c_cyan); putstr(nl); putstr(nl);                                                                              \
	putstr(general_info);                                                                                                \
	REGROUPU64(ia,input_code.ptr,nl);                                                                                    \
	REGROUP32(is,input_code.size,nl);                                                                                    \
	REGROUPU64(ba,output_buffer.ptr,nl);                                                                                 \
	REGROUP32(bs,output_buffer.size,nl);                                                                                 \
	REGROUPU64(ep,entry_point,nl);                                                                                       \
	REGROUP32(fc,virus_func_shift,nl);                                                                                   \
	REGROUP32(os,output_size,nl);                                                                                        \
	REGROUPU64(sd,seed,nl);                                                                                              \
	putstr(c_none); putstr(nl);                                                                                          \
})

# else
#  define debug_print_original_block(...)
#  define debug_print_split_blocks(...)
#  define debug_print_general(...)
# endif

#endif
