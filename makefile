# program
## program name
NAME = globox
## program execution command
CMD = ./$(NAME)
## valgrind execution arguments
VALGRIND = --show-leak-kinds=all --track-origins=yes --leak-check=full --suppressions=../res/valgrind.supp

# compiler
## compiler name
CC = gcc
## compiler options
FLAGS = -std=c99 -pedantic -g
## warning settings
FLAGS+= -Wall -Wextra -Werror=vla -Werror -Wno-unused-parameter -Wno-address-of-packed-member

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
SRCS_OBJS = $(OBJD)/$(RESD)/icon/iconpix.o
## object files list
SRCS_OBJS+= $(patsubst %.c,$(OBJD)/%.o,$(SRCS))

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
