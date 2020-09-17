# program
## program name
NAME = globox
## program execution command
CMD = ./$(NAME)
## valgrind execution arguments
VALGRIND = --show-error-list=yes --show-leak-kinds=all --track-origins=yes --leak-check=full --suppressions=../res/valgrind.supp

# compiler
## compiler name
CC = gcc
## compiler options
FLAGS = -std=c99 -pedantic -g
## warning settings
FLAGS+= -Wall -Wextra -Werror=vla -Werror -Wno-unused-parameter -Wno-address-of-packed-member
## error settings
FLAGS+= -DGLOBOX_ERROR_LOG_BASIC
FLAGS+= -DGLOBOX_ERROR_LOG_THROW
FLAGS+= -DGLOBOX_ERROR_LOG_DEBUG
#FLAGS+= -DGLOBOX_ERROR_SKIP
#FLAGS+= -DGLOBOX_ERROR_ABORT

# source
## folders
BIND = bin
OBJD = obj
SRCD = src
INCD = inc
SUBD = sub
RESD = res
## header files
INCL = -I$(SRCD)
INCL+= -I$(INCD)
## main code files
SRCS = $(SRCD)/globox.c
SRCS+= $(SRCD)/globox_error.c
SRCS_OBJS = $(OBJD)/$(RESD)/icon/iconpix.o

# targets
PLATFORM ?= X11
CONTEXT ?= SOFTWARE

ifeq ($(PLATFORM), X11)
SRCS+= $(SRCD)/x11/globox_x11.c
LINK+= -lxcb
FLAGS+= -DGLOBOX_PLATFORM_X11

ifeq ($(CONTEXT), SOFTWARE)
FLAGS+= -DGLOBOX_CONTEXT_SOFTWARE
SRCS+= example/software.c
SRCS+= $(SRCD)/x11/software/globox_x11_software.c
LINK+= -lxcb-shm
LINK+= -lxcb-randr
endif

ifeq ($(CONTEXT), EGL)
FLAGS+= -DGLOBOX_CONTEXT_EGL
SRCS+= example/egl.c
SRCS+= $(SRCD)/x11/egl/globox_x11_egl.c
LINK+= -lGL
LINK+= -lEGL
endif
endif

# object files list
SRCS_OBJS+= $(patsubst %.c,$(OBJD)/%.o,$(SRCS))

# bin
.PHONY: final
final: $(BIND)/$(NAME)

## icon
$(RESD)/icon/iconpix.bin:
	@echo "generating icons pixmap"
	@cd $(RESD)/icon && ./makepix.sh

$(OBJD)/$(RESD)/icon/iconpix.o: $(RESD)/icon/iconpix.bin
	@echo "building icon object"
	@mkdir -p $(@D)
	@objcopy -I binary -O elf64-x86-64 -B i386:x86-64 \
	--redefine-syms=$(RESD)/icon/syms.map \
	--rename-section .data=.iconpix \
	$< $@

## compilation
$(OBJD)/%.o: %.c
	@echo "building object $@"
	@mkdir -p $(@D)
	@$(CC) $(INCL) $(FLAGS) -c -o $@ $<

$(BIND)/$(NAME): $(SRCS_OBJS)
	@echo "compiling executable $@"
	@mkdir -p $(@D)
	@$(CC) -o $@ $^ $(LINK)

# tools
## valgrind memory leak detection
leak: $(BIND)/$(NAME)
	@echo "# running valgrind"
	rm -f valgrind.log
	cd $(BIND) && valgrind $(VALGRIND) 2> ../valgrind.log $(CMD)
	less valgrind.log
## repository cleaning
clean:
	@echo "# cleaning"
	rm -rf $(BIND) $(OBJD) valgrind.log
## running
run:
	@cd $(BIND) && $(CMD)
## remotes edition
remotes:
	@echo "# registering remotes"
	git remote add gitea ssh://git@git.nullgemm.fr:2999/nullgemm/$(NAME).git
	git remote add github git@github.com:nullgemm/$(NAME).git
	git remote remove origin
## submodules update
github:
	@echo "# sourcing submodules from https://github.com"
	cp .github .gitmodules
	git submodule sync
	git submodule update --init --remote
## submodules update
gitea:
	@echo "# sourcing submodules from personal server"
	cp .gitea .gitmodules
	git submodule sync
	git submodule update --init --remote
