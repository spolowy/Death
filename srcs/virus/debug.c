
#if defined(DEBUG_OPERANDS) || defined(DEBUG_BLOCKS)

/* ########################################################################## */
/* ################################## Utils ################################# */
/* ########################################################################## */

/* --------------------------------- Colors --------------------------------- */

# define RED		'\033','[','0',';','3','1','m'
# define GREEN		'\033','[','0',';','3','2','m'
# define YELLOW		'\033','[','0',';','3','3','m'
# define BLUE		'\033','[','0',';','3','4','m'
# define MAGENTA	'\033','[','0',';','3','5','m'
# define CYAN		'\033','[','0',';','3','6','m'
# define GREY		'\033','[','1',';','3','0','m'
# define NONE		'\033','[','0','m'

/* -------------------------------- Strings --------------------------------- */

# define NL		'\n'
# define SP		' '

/* ########################################################################## */
/* ############################## Debug blocks ############################## */
/* ########################################################################## */

# ifdef DEBUG_BLOCKS

#  include "position_independent.h"
#  include "utils.h"
#  include "accessors.h"
#  include "disasm.h"
#  include "blocks.h"

// /* -------------------------------------------------------------------------- */
// /* ---------------------------------- Utils --------------------------------- */
// /* -------------------------------------------------------------------------- */

/* -------------------------------- Regroups -------------------------------- */

#  define REGROUPU64(s1, n, s2) ({                                             \
	putstr(s1);                                                            \
	putu64((uint64_t)n);                                                   \
	putstr(s2);                                                            \
})

#  define REGROUPS32(s1, n, s2) ({                                             \
	putstr(s1);                                                            \
	puts32((int32_t)n);                                                    \
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

static void	original_label(const struct label *label)
{
	PD_ARRAY(char,nl,NL,0);
	PD_ARRAY(char,sp,SP,0);
	PD_ARRAY(char,vcb_labels_addr,GREEN,'a','d','d','r','e','s','s',NONE,':',' ',RED,0)
	PD_ARRAY(char,vcb_labels_jumps,GREEN,'j','u','m','p','s',NONE,':',' ',RED,0)
	PD_ARRAY(char,vcb_labels_njumps,GREEN,'n','j','u','m','p','s',NONE,':',' ',RED,0)
	PD_ARRAY(char,vcb_labels_jump,RED,0)
	PD_ARRAY(char,sep,GREY,'/',0)

	REGROUPU64(vcb_labels_addr,label->location,sp);
	REGROUPS32(vcb_labels_njumps,label->njumps,sp);
	putstr(vcb_labels_jumps);

	struct jump	**p_jumps = label->jumps;
	for (size_t n = 0; n < label->njumps; n++)
	{
		struct jump *p_jump = (struct jump*)(p_jumps[n]);
		REGROUPU64(vcb_labels_jump,p_jump->location,"");
		if (n + 1 < label->njumps && p_jump + 1 != NULL)
			putstr(sep);
	}
	putstr(nl);
}

static void 	print_original_labels(const struct label *labels, size_t nlabels)
{
	PD_ARRAY(char,nl,NL,0);
	PD_ARRAY(char,vcb_labels,BLUE,' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','l','a','b','e','l','s',NL,NL,NONE,0);

	putstr(vcb_labels);
	const struct label	*p_labels = labels;
	for (size_t n = 0; n < nlabels && p_labels; n++)
	{
		original_label(p_labels);
		p_labels++;
	}
	putstr(nl);
}

/* --------------------------------- Jumps ---------------------------------- */

static void	original_jump(const struct jump *jump)
{
	PD_ARRAY(char,nl,NL,0);
	PD_ARRAY(char,sp,SP,0);
	PD_ARRAY(char,vcb_jump_addr,GREEN,'a','d','d','r','e','s','s',NONE,':',' ',RED,0)
	PD_ARRAY(char,vcb_jump_target,GREEN,'s','i','z','e',NONE,':',' ',RED,0)
	PD_ARRAY(char,vcb_jump_value_addr,GREEN,'v','a','l','u','e','_','a','d','d','r','e','s','s',NONE,':',' ',RED,0)
	PD_ARRAY(char,vcb_jump_value_size,GREEN,'v','a','l','u','e','_','s','i','z','e',NONE,':',' ',RED,0)
	PD_ARRAY(char,vcb_jump_displacement,GREEN,'v','a','l','u','e',NONE,':',' ',RED,0)
	PD_ARRAY(char,vcb_jump_shift_amount,GREEN,'s','h','i','f','t','_','a','m','o','u','n','t',NONE,':',' ',RED,0)

	REGROUPU64(vcb_jump_addr,jump->location,sp);
	REGROUPS32(vcb_jump_target,jump->instruction_size,sp);
	REGROUPU64(vcb_jump_value_addr,jump->value_addr,sp);
	REGROUPS32(vcb_jump_value_size,jump->value_size,sp);
	REGROUPS32(vcb_jump_displacement,jump->value,sp);
	REGROUPS32(vcb_jump_shift_amount,jump->value_shift,nl);
}

static void	print_original_jumps(const struct jump *jumps, size_t njumps)
{
	PD_ARRAY(char,nl,NL,0);
	PD_ARRAY(char,vcb_jumps,BLUE,' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','j','u','m','p','s',NL,NL,0);

	putstr(vcb_jumps);

	const struct jump	*p_jumps = jumps;
	for (size_t n = 0; n < njumps && p_jumps; n++)
	{
		original_jump(p_jumps);
		p_jumps++;
	}
	putstr(nl);
}

/* ------------------------------- Code Block ------------------------------- */

static void	original_code_block(const struct code_block *block)
{
	PD_ARRAY(char,nl,NL,0);
	PD_ARRAY(char,vcb,BLUE,' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','c','o','d','e',' ','b','l','o','c','k',NL,NONE,0);
	PD_ARRAY(char,vcb_addr,GREEN,'a','d','d','r','e','s','s',NONE,':',' ',RED,0)
	PD_ARRAY(char,vcb_size,GREEN,'s','i','z','e',' ',' ',' ',NONE,':',' ',RED,0)
	PD_ARRAY(char,vcb_njumps,GREEN,'n','j','u','m','p','s',' ',NONE,':',' ',RED,0)
	PD_ARRAY(char,vcb_nlabels,GREEN,'n','l','a','b','e','l','s',NONE,':',' ',RED,0)
	PD_ARRAY(char,vcb_shift_amount,GREEN,'s','h','i','f','t','_','a','m','o','u','n','t',NONE,':',' ',RED,0)

	putstr(vcb);
	REGROUPU64(vcb_addr,block->ref.ptr,nl);
	REGROUPS32(vcb_size,block->ref.size,nl);
	REGROUPS32(vcb_njumps,block->njumps,nl);
	REGROUPS32(vcb_nlabels,block->nlabels,nl);
	REGROUPS32(vcb_shift_amount,block->shift_amount,nl);
}

static void	print_original_block(const struct code_block *block)
{
	original_code_block(block);
	print_original_jumps(block->jumps, block->njumps);
	print_original_labels(block->labels, block->nlabels);
}

void	debug_print_original_block(const struct code_block *blocks)
{
	PD_ARRAY(char,nl,NL,0);
	PD_ARRAY(char,none,NONE,0)
	PD_ARRAY(char,original_block,CYAN,'#',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','#',NL,'#',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','O','r','i','g','i','n','a','l',' ','B','l','o','c','k',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','#',NL,'#',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','#',NONE,NL,NL,0);

	putstr(nl); putstr(nl);
	putstr(original_block);
	print_original_block(blocks);
	putstr(none);
}

/* -------------------------------------------------------------------------- */
/* --------------------------- Print Split Blocks --------------------------- */
/* -------------------------------------------------------------------------- */

/* --------------------------------- Header --------------------------------- */

static void	print_head(size_t n)
{
	PD_ARRAY(char,nl,NL,0);
	PD_ARRAY(char,vnb_s1,MAGENTA,'[','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-',' ','b','l','o','c','k',' ',0)
	PD_ARRAY(char,vnb_s2,' ','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-',']',NL,NL,NONE,0)

	REGROUPS32(vnb_s1,n,vnb_s2);
	putstr(nl);
}

/* --------------------------------- Labels --------------------------------- */

static void	split_labels(const struct label *label,
			size_t shift_amount,
			struct safe_ptr input_code, struct safe_ptr output_buffer)
{
	PD_ARRAY(char,nl,NL,0);
	PD_ARRAY(char,sp,SP,0);
	PD_ARRAY(char,vcb_labels_addr,GREEN,'a','d','d','r','e','s','s',NONE,':',' ',RED,0)
	PD_ARRAY(char,vcb_labels_jumps,GREEN,'j','u','m','p','s',NONE,':',' ',RED,0)
	PD_ARRAY(char,vcb_labels_njumps,GREEN,'n','j','u','m','p','s',NONE,':',' ',RED,0)
	PD_ARRAY(char,vcb_labels_jump,RED,0)
	PD_ARRAY(char,sep,GREY,'/',0)
	PD_ARRAY(char,vcb_arrow,GREY,'-','>',0)
	PD_ARRAY(char,vcb_red,RED,0)

	REGROUPU64_ARROW(vcb_labels_addr,label->location,vcb_arrow,vcb_red,IN_OUT(label->location,input_code,output_buffer,shift_amount),sp);
	REGROUPS32(vcb_labels_njumps,label->njumps,sp);
	putstr(vcb_labels_jumps);
	struct jump	**p_jumps = label->jumps;
	for (size_t n = 0; n < label->njumps; n++)
	{
		struct jump *p_jump = (struct jump*)(p_jumps[n]);
		REGROUPU64_ARROW(vcb_labels_jump,p_jump->location,vcb_arrow,vcb_red,IN_OUT(p_jump->location,input_code,output_buffer,shift_amount),"");
		if (n + 1 < label->njumps && p_jump + 1 != NULL)
			putstr(sep);
	}
	putstr(nl);
}

static void	print_split_labels(const struct label *labels, size_t nlabels,
			size_t shift_amount,
			struct safe_ptr input_code, struct safe_ptr output_buffer)
{
	PD_ARRAY(char,nl,NL,0);
	PD_ARRAY(char,vcb_labels,BLUE,' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','l','a','b','e','l','s',NL,NL,NONE,0);

	putstr(vcb_labels);
	const struct label	*p_labels = labels;
	for (size_t n = 0; n < nlabels && p_labels; n++)
	{
		split_labels(p_labels, shift_amount, input_code, output_buffer);
		p_labels++;
	}
	putstr(nl);
}

/* --------------------------------- Jumps ---------------------------------- */

static void	split_jumps(const struct jump *jump,
			size_t shift_amount,
			struct safe_ptr input_code, struct safe_ptr output_buffer)
{
	PD_ARRAY(char,nl,NL,0);
	PD_ARRAY(char,sp,SP,0);
	PD_ARRAY(char,vcb_jump_addr,GREEN,'a','d','d','r','e','s','s',NONE,':',' ',RED,0)
	PD_ARRAY(char,vcb_jump_target,GREEN,'s','i','z','e',NONE,':',' ',RED,0)
	PD_ARRAY(char,vcb_jump_value_addr,GREEN,'v','a','l','u','e','_','a','d','d','r','e','s','s',NONE,':',' ',RED,0)
	PD_ARRAY(char,vcb_jump_value_size,GREEN,'v','a','l','u','e','_','s','i','z','e',NONE,':',' ',RED,0)
	PD_ARRAY(char,vcb_jump_displacement,GREEN,'v','a','l','u','e',NONE,':',' ',RED,0)
	PD_ARRAY(char,vcb_jump_shift_amount,GREEN,'s','h','i','f','t','_','a','m','o','u','n','t',NONE,':',' ',RED,0)
	PD_ARRAY(char,vcb_arrow,GREY,'-','>',0)
	PD_ARRAY(char,vcb_red,RED,0)

	REGROUPU64_ARROW(vcb_jump_addr,jump->location,vcb_arrow,vcb_red,IN_OUT(jump->location,input_code,output_buffer,shift_amount),sp);
	REGROUPS32(vcb_jump_target,jump->instruction_size,sp);
	REGROUPU64_ARROW(vcb_jump_value_addr,jump->value_addr,vcb_arrow,vcb_red,IN_OUT(jump->value_addr,input_code,output_buffer,shift_amount),sp);
	REGROUPS32(vcb_jump_value_size,jump->value_size,sp);
	REGROUPS32(vcb_jump_displacement,jump->value,sp);
	REGROUPS32(vcb_jump_shift_amount,jump->value_shift,nl);
}

static void	print_split_jumps(const struct jump *jumps, size_t njumps,
			size_t shift_amount,
			struct safe_ptr input_code, struct safe_ptr output_buffer)
{
	PD_ARRAY(char,nl,NL,0);
	PD_ARRAY(char,vcb_jumps,BLUE,' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','j','u','m','p','s',NL,NL,0);

	putstr(vcb_jumps);
	const struct jump	*p_jumps = jumps;
	for (size_t n = 0; n < njumps && p_jumps; n++)
	{
		split_jumps(p_jumps, shift_amount, input_code, output_buffer);
		p_jumps++;
	}
	putstr(nl);
}

/* ------------------------------- Code Block ------------------------------- */

static void	split_code_block(const struct code_block *block,
			size_t shift_amount,
			struct safe_ptr input_code, struct safe_ptr output_buffer)
{
	PD_ARRAY(char,nl,NL,0);
	PD_ARRAY(char,vcb,BLUE,' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','c','o','d','e',' ','b','l','o','c','k',NL,NL,NONE,0);
	PD_ARRAY(char,vcb_addr,GREEN,'a','d','d','r','e','s','s',NONE,':',' ',RED,0)
	PD_ARRAY(char,vcb_size,GREEN,'s','i','z','e',' ',' ',' ',NONE,':',' ',RED,0)
	PD_ARRAY(char,vcb_njumps,GREEN,'n','j','u','m','p','s',' ',NONE,':',' ',RED,0)
	PD_ARRAY(char,vcb_nlabels,GREEN,'n','l','a','b','e','l','s',NONE,':',' ',RED,0)
	PD_ARRAY(char,vcb_shift_amount,GREEN,'s','h','i','f','t','_','a','m','o','u','n','t',NONE,':',' ',RED,0)
	PD_ARRAY(char,vcb_trailing_block,GREEN,'t','r','a','i','l','i','n','g','_','b','l','o','c','k', NONE,':',' ',RED,0)
	PD_ARRAY(char,vcb_arrow,GREY,'-','>',0)
	PD_ARRAY(char,vcb_red,RED,0)

	putstr(vcb);
	REGROUPU64_ARROW(vcb_addr,block->ref.ptr,vcb_arrow,vcb_red,IN_OUT(block->ref.ptr,input_code,output_buffer,shift_amount),nl);
	REGROUPS32(vcb_size,block->ref.size,nl);
	REGROUPS32(vcb_njumps,block->njumps,nl);
	REGROUPS32(vcb_nlabels,block->nlabels,nl);
	REGROUPS32(vcb_shift_amount,block->shift_amount,nl);

	if (block->trailing_block)
		REGROUPU64_ARROW(vcb_trailing_block,block->ref.ptr,vcb_arrow,vcb_red,IN_OUT(block->trailing_block->ref.ptr,input_code,output_buffer,block->trailing_block->shift_amount),nl);
	else
		REGROUPU64(vcb_trailing_block,block->trailing_block,nl);
}

static void	print_split_blocks(const struct code_block *blocks, size_t nblocks,
		struct safe_ptr input_code, struct safe_ptr output_buffer)
{
	PD_ARRAY(char,nl,NL,0);

	const struct code_block	*p_blocks = blocks;
	for (size_t nb = 0; nb < nblocks && p_blocks; nb++)
	{
		print_head(nb);
		split_code_block(p_blocks, p_blocks->shift_amount, input_code, output_buffer);
		print_split_jumps(p_blocks->jumps, p_blocks->njumps, p_blocks->shift_amount, input_code, output_buffer);
		print_split_labels(p_blocks->labels, p_blocks->nlabels, p_blocks->shift_amount, input_code, output_buffer);
		p_blocks++;
	}
}

/* ------------------------------- Code Block ------------------------------- */

void	debug_print_split_blocks(const struct code_block *blocks, size_t nblocks,
		struct safe_ptr input_code, struct safe_ptr output_buffer)
{
	PD_ARRAY(char,nl,NL,0);
	PD_ARRAY(char,none,NONE,0)
	PD_ARRAY(char,split_blocks,CYAN,'#',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','#',NL,'#',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','S','p','l','i','t',' ','B','l','o','c','k','s',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','#',NL,'#',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','#',NL,NL,0)

	putstr(nl); putstr(nl);
	putstr(split_blocks);
	print_split_blocks(blocks, nblocks, input_code, output_buffer);
	putstr(none);
}

/* -------------------------------------------------------------------------- */
/* --------------------------- Print General Info --------------------------- */
/* -------------------------------------------------------------------------- */

void	debug_print_general_block(struct safe_ptr input_code,
		struct safe_ptr output_buffer, size_t entry_point,
		int32_t virus_func_shift, size_t output_size,
		uint64_t seed)
{
	PD_ARRAY(char,nl,NL,0);
	PD_ARRAY(char,ia,YELLOW,'i','n','p','u','t','_','a','d','d','r', NONE,':',' ',RED,0)
	PD_ARRAY(char,is,YELLOW,'i','n','p','u','t','_','s','i','z','e', NONE,':',' ',RED,0)
	PD_ARRAY(char,ba,YELLOW,'b','u','f','f','e','r','_','a','d','d','r', NONE,':',' ',RED,0)
	PD_ARRAY(char,bs,YELLOW,'b','u','f','f','e','r','_','s','i','z','e', NONE,':',' ',RED,0)
	PD_ARRAY(char,ep,YELLOW,'e','n','t','r','y','_','p','o','i','n','t', NONE,':',' ',RED,0)
	PD_ARRAY(char,fc,YELLOW,'v','i','r','u','s','_','f','u','n','c','_','s','h','i','f','t', NONE,':',' ',RED,0)
	PD_ARRAY(char,os,YELLOW,'o','u','t','p','u','t','_','s','i','z','e', NONE,':',' ',RED,0)
	PD_ARRAY(char,sd,YELLOW,'s','e','e','d', NONE,':',' ',RED,0)
	PD_ARRAY(char,cyan,CYAN,0)
	PD_ARRAY(char,none,NONE,0)
	PD_ARRAY(char,general_info,'#',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','#','\n','#',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','G','e','n','e','r','a','l',' ','I','n','f','o',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','#','\n','#',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','#',NL,NL,0);

	putstr(nl); putstr(nl);
	putstr(cyan); putstr(general_info);
	REGROUPU64(ia,input_code.ptr,nl);
	REGROUPS32(is,input_code.size,nl);
	REGROUPU64(ba,output_buffer.ptr,nl);
	REGROUPS32(bs,output_buffer.size,nl);
	REGROUPU64(ep,entry_point,nl);
	REGROUPS32(fc,virus_func_shift,nl);
	REGROUPS32(os,output_size,nl);
	REGROUPU64(sd,seed,nl);
	putstr(none); putstr(nl);
}
# endif

/* ########################################################################## */
/* ############################# Debug operands ############################# */
/* ########################################################################## */

# ifdef DEBUG_OPERANDS

#  include "disasm.h"
#  include "utils.h"
#  include "position_independent.h"

static void	print_masks(uint32_t reg)
{
	PD_ARRAY(char,unknown,YELLOW,'u','n','k','n','o','w','n',' ',NONE,0)
	PD_ARRAY(char,rax,GREEN,'r','a','x',' ',NONE,0)
	PD_ARRAY(char,rbx,GREEN,'r','b','x',' ',NONE,0)
	PD_ARRAY(char,rcx,GREEN,'r','c','x',' ',NONE,0)
	PD_ARRAY(char,rdx,GREEN,'r','d','x',' ',NONE,0)
	PD_ARRAY(char,rdi,GREEN,'r','d','i',' ',NONE,0)
	PD_ARRAY(char,rsi,GREEN,'r','s','i',' ',NONE,0)
	PD_ARRAY(char,r8,GREEN,'r','8',' ',NONE,0)
	PD_ARRAY(char,r9,GREEN,'r','9',' ',NONE,0)
	PD_ARRAY(char,r10,GREEN,'r','1','0',' ',NONE,0)
	PD_ARRAY(char,r11,GREEN,'r','1','1',' ',NONE,0)
	PD_ARRAY(char,r12,GREEN,'r','1','2',' ',NONE,0)
	PD_ARRAY(char,r13,GREEN,'r','1','3',' ',NONE,0)
	PD_ARRAY(char,r14,GREEN,'r','1','4',' ',NONE,0)
	PD_ARRAY(char,r15,GREEN,'r','1','5',' ',NONE,0)
	PD_ARRAY(char,rbp,GREEN,'r','b','p',' ',NONE,0)
	PD_ARRAY(char,rsp,GREEN,'r','s','p',' ',NONE,0)
	PD_ARRAY(char,rip,GREEN,'r','i','p',' ',NONE,0)
	PD_ARRAY(char,memory,GREEN,'m','e','m','o','r','y',' ',NONE,0)
	PD_ARRAY(char,flags,GREEN,'f','l','a','g','s',' ',NONE,0)
	PD_ARRAY(char,none,GREEN,'n','o','n','e',' ',NONE,0)

	if (reg == (uint32_t)UNKNOWN) {putstr(unknown);}
	else
	{
		if (reg & RAX)        {putstr(rax);}
		if (reg & RBX)        {putstr(rbx);}
		if (reg & RCX)        {putstr(rcx);}
		if (reg & RDX)        {putstr(rdx);}
		if (reg & RDI)        {putstr(rdi);}
		if (reg & RSI)        {putstr(rsi);}
		if (reg & R8)         {putstr(r8);}
		if (reg & R9)         {putstr(r9);}
		if (reg & R10)        {putstr(r10);}
		if (reg & R11)        {putstr(r11);}
		if (reg & R12)        {putstr(r12);}
		if (reg & R13)        {putstr(r13);}
		if (reg & R14)        {putstr(r14);}
		if (reg & R15)        {putstr(r15);}
		if (reg & RBP)        {putstr(rbp);}
		if (reg & RSP)        {putstr(rsp);}
		if (reg & RIP)        {putstr(rip);}
		if (reg & MEMORY)     {putstr(memory);}
		if (reg & FLAGS)      {putstr(flags);}
		if (reg == RNO)       {putstr(none);}
	}
}

void	debug_print_operands(const struct operand *instructions, size_t n)
{
	PD_ARRAY(char,nl,NL,0);
	PD_ARRAY(char,hex,'0','x',0);
	PD_ARRAY(char,col,':','\t',0);
	PD_ARRAY(char,com,',',' ',0);
	for (size_t i = 0; i < n; i++)
	{
		putstr(hex); putu64((size_t)instructions[i].addr);
		putstr(col);
		print_masks(instructions[i].dst); putstr(com);
		print_masks(instructions[i].src); putstr(nl);
	}
}
# endif

#endif
