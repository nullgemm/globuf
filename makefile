# program
## program name
NAME = globox
## program execution command
CMD = ./$(NAME)
# targets
PLATFORM ?= WINDOWS
CONTEXT ?= SOFTWARE
NATIVE ?= TRUE
## valgrind execution arguments
VALGRIND = --show-error-list=yes --show-leak-kinds=all --track-origins=yes --leak-check=full --suppressions=../res/valgrind.supp

# compiler
## compiler name
CC = gcc
OBJCOPY = objcopy
## compiler options
FLAGS = -std=c99 -pedantic -g
## warning settings
FLAGS+= -Wall -Wextra -Werror=vla -Werror -Wno-unused-parameter -Wno-address-of-packed-member
## windows-specific
WINDOWS_VERSION = 10
WINDOWS_VERSION_SDK = 10.0.19041.0
WINDOWS_VERSION_MSVC = 14.28.29333
WINDOWS_VERSION_VISUAL_STUDIO = 2019

ifeq ($(PLATFORM), WINDOWS)
ifeq ($(NATIVE), TRUE)
## remove gcc flags
FLAGS =
## include SDK directories
INCL+= -I"/c/Program Files (x86)/Windows Kits/$\
$(WINDOWS_VERSION)/Include/$\
$(WINDOWS_VERSION_SDK)/ucrt"
INCL+= -I"/c/Program Files (x86)/Windows Kits/$\
$(WINDOWS_VERSION)/Include/$\
$(WINDOWS_VERSION_SDK)/um"
INCL+= -I"/c/Program Files (x86)/Windows Kits/$\
$(WINDOWS_VERSION)/Include/$\
$(WINDOWS_VERSION_SDK)/shared"
INCL+= -I"/c/Program Files (x86)/Microsoft Visual Studio/$\
$(WINDOWS_VERSION_VISUAL_STUDIO)/BuildTools/VC/Tools/MSVC/$\
$(WINDOWS_VERSION_MSVC)/include"
endif
endif

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
INCL+= -I$(SRCD)
INCL+= -I$(INCD)
## main code files
SRCS = $(SRCD)/globox.c
SRCS+= $(SRCD)/globox_error.c
SRCS_OBJS =

## X11
ifeq ($(PLATFORM), X11)
SRCS+= $(SRCD)/x11/globox_x11.c
SRCS_OBJS+= $(OBJD)/$(RESD)/icon/iconpix.o
LINK+= `pkg-config xcb --cflags --libs`
FLAGS+= -DGLOBOX_PLATFORM_X11

ifeq ($(CONTEXT), SOFTWARE)
FLAGS+= -DGLOBOX_CONTEXT_SOFTWARE
SRCS+= example/software.c
SRCS+= $(SRCD)/x11/software/globox_x11_software.c
LINK+= `pkg-config xcb-shm xcb-randr xcb-render --cflags --libs`
endif

ifeq ($(CONTEXT), EGL)
FLAGS+= -DGLOBOX_CONTEXT_EGL
SRCS+= example/egl.c
SRCS+= $(SRCD)/x11/egl/globox_x11_egl.c
LINK+= `pkg-config egl glesv2 --cflags --libs`
endif

ifeq ($(CONTEXT), GLX)
FLAGS+= -DGLOBOX_CONTEXT_GLX
SRCS+= example/glx.c
SRCS+= $(SRCD)/x11/glx/globox_x11_glx.c
LINK+= `pkg-config glx glesv2 --cflags --libs`
LINK+= `pkg-config x11 x11-xcb xrender --cflags --libs`
endif
endif

## WAYLAND
ifeq ($(PLATFORM), WAYLAND)
SRCS+= $(SRCD)/wayland/globox_wayland.c
SRCS+= $(SRCD)/wayland/globox_wayland_callbacks.c
SRCS+= $(INCD)/xdg-shell-protocol.c
SRCS+= $(INCD)/xdg-decoration-protocol.c
SRCS+= $(INCD)/kde-blur-protocol.c
SRCS+= $(INCD)/zwp-relative-pointer-protocol.c
SRCS+= $(INCD)/zwp-pointer-constraints-protocol.c
SRCS_OBJS+= $(OBJD)/$(RESD)/icon/iconpix.o
LINK+= `pkg-config wayland-client --cflags --libs`
LINK+= -lrt
FLAGS+= -DGLOBOX_PLATFORM_WAYLAND

ifeq ($(CONTEXT), SOFTWARE)
FLAGS+= -DGLOBOX_CONTEXT_SOFTWARE
SRCS+= example/software.c
SRCS+= $(SRCD)/wayland/software/globox_wayland_software.c
SRCS+= $(SRCD)/wayland/software/globox_wayland_software_helpers.c
endif

ifeq ($(CONTEXT), EGL)
FLAGS+= -DGLOBOX_CONTEXT_EGL
SRCS+= example/egl.c
SRCS+= $(SRCD)/wayland/egl/globox_wayland_egl.c
SRCS+= $(SRCD)/wayland/egl/globox_wayland_egl_helpers.c
LINK+= `pkg-config wayland-egl egl glesv2 --cflags --libs`
endif
endif

## WINDOWS
ifeq ($(PLATFORM), WINDOWS)
SRCS+= $(SRCD)/windows/globox_windows.c
SRCS_OBJS+= $(OBJD)/$(RESD)/icon/iconpix.obj
FLAGS+= -DGLOBOX_PLATFORM_WINDOWS -DUNICODE -D_UNICODE

ifeq ($(NATIVE), FALSE)
CMD = ./$(NAME).exe
else
CMD = ./$(NAME)_msvc.exe
endif

ifeq ($(NATIVE), FALSE)
CC = x86_64-w64-mingw32-gcc
LINK+= -mwindows
else
CC = "/c/Program Files (x86)/Microsoft Visual Studio/$\
$(WINDOWS_VERSION_VISUAL_STUDIO)/BuildTools/VC/Tools/MSVC/$\
$(WINDOWS_VERSION_MSVC)/bin/Hostx64/x64/cl.exe"
LINK_WINDOWS+= -LIBPATH:"/c/Program Files (x86)/Windows Kits/$\
$(WINDOWS_VERSION)/Lib/$\
$(WINDOWS_VERSION_SDK)/um/x64"
LINK_WINDOWS+= -LIBPATH:"/c/Program Files (x86)/Microsoft Visual Studio/$\
$(WINDOWS_VERSION_VISUAL_STUDIO)/BuildTools/VC/Tools/MSVC/$\
$(WINDOWS_VERSION_MSVC)/lib/spectre/x64"
LINK_WINDOWS+= -LIBPATH:"/c/Program Files (x86)/Windows Kits/$\
$(WINDOWS_VERSION)/Lib/$\
$(WINDOWS_VERSION_SDK)/ucrt/x64"
endif

ifeq ($(CONTEXT), SOFTWARE)
FLAGS+= -DGLOBOX_CONTEXT_SOFTWARE
SRCS+= example/software.c
SRCS+= $(SRCD)/windows/software/globox_windows_software.c
ifeq ($(NATIVE), FALSE)
LINK+= -lgdi32
else
LINK = -l Gdi32.lib User32.lib shcore.lib
endif
endif

ifeq ($(CONTEXT), WGL)
FLAGS+= -DGLOBOX_CONTEXT_WGL
SRCS+= example/wgl.c
SRCS+= $(SRCD)/windows/software/globox_windows_wgl.c
ifeq ($(NATIVE), FALSE)
LINK+= -lopengl32
else
LINK = -l Gdi32.lib User32.lib shcore.lib
endif
endif
endif

## MACOS
ifeq ($(PLATFORM), MACOS)
CMD = ./$(NAME).app
SRCS+= $(SRCD)/macos/globox_macos.c
SRCS+= $(SRCD)/macos/globox_macos_symbols.c
SRCS+= $(SRCD)/macos/globox_macos_callbacks.c
SRCS_OBJS+= $(OBJD)/$(RESD)/icon/iconpix_mach.o
LINK+= -framework AppKit
FLAGS+= -DGLOBOX_PLATFORM_MACOS

ifeq ($(NATIVE), FALSE)
CC = o64-clang
else
OBJCOPY = /usr/local/Cellar/binutils/*/bin/objcopy
endif

ifeq ($(CONTEXT), SOFTWARE)
FLAGS+= -DGLOBOX_CONTEXT_SOFTWARE
SRCS+= example/software.c
SRCS+= $(SRCD)/macos/software/globox_macos_software.c
endif

ifeq ($(CONTEXT), EGL)
FLAGS+= -DGLOBOX_CONTEXT_EGL
SRCS+= example/egl.c
SRCS+= $(SRCD)/macos/egl/globox_macos_egl.c
INCL+= -I$(RESD)/angle/include
LINK+= -L$(RESD)/angle/libs
LINK+= -lEGL -lGLESv2
endif
endif

# object files list
ifeq ($(PLATFORM), WINDOWS)
SRCS_OBJS+= $(patsubst %.c,$(OBJD)/%.obj,$(SRCS))
else
SRCS_OBJS+= $(patsubst %.c,$(OBJD)/%.o,$(SRCS))
endif

# bin
.PHONY: final

ifeq ($(PLATFORM), WAYLAND)
final: $(BIND)/$(NAME)
endif

ifeq ($(PLATFORM), X11)
final: $(BIND)/$(NAME)
endif

ifeq ($(PLATFORM), WINDOWS)
ifeq ($(NATIVE), FALSE)
final: $(BIND)/$(NAME).exe
else
final: $(BIND)/$(NAME)_msvc.exe
endif
endif

ifeq ($(PLATFORM), MACOS)
final: $(BIND)/$(NAME).app
endif

## wayland protocols
ifeq ($(PLATFORM), WAYLAND)
$(INCD):
	@echo "generating wayland protocol extensions source files"
	@mkdir $@
	@wayland-scanner private-code \
	< /usr/share/wayland-protocols/stable/xdg-shell/xdg-shell.xml \
	> $@/xdg-shell-protocol.c
	@wayland-scanner client-header \
	< /usr/share/wayland-protocols/stable/xdg-shell/xdg-shell.xml \
	> $@/xdg-shell-client-protocol.h
	@wayland-scanner private-code \
	< /usr/share/wayland-protocols/unstable/xdg-decoration/xdg-decoration-unstable-v1.xml \
	> $@/xdg-decoration-protocol.c
	@wayland-scanner client-header \
	< /usr/share/wayland-protocols/unstable/xdg-decoration/xdg-decoration-unstable-v1.xml \
	> $@/xdg-decoration-client-protocol.h
	@wayland-scanner private-code \
	< /usr/share/plasma-wayland-protocols/blur.xml \
	> $@/kde-blur-protocol.c
	@wayland-scanner client-header \
	< /usr/share/plasma-wayland-protocols/blur.xml \
	> $@/kde-blur-client-protocol.h
	@wayland-scanner private-code \
	< /usr/share/wayland-protocols/unstable/pointer-constraints/pointer-constraints-unstable-v1.xml \
	> $@/zwp-pointer-constraints-protocol.c
	@wayland-scanner client-header \
	< /usr/share/wayland-protocols/unstable/pointer-constraints/pointer-constraints-unstable-v1.xml \
	> $@/zwp-pointer-constraints-protocol.h
	@wayland-scanner private-code \
	< /usr/share/wayland-protocols/unstable/relative-pointer/relative-pointer-unstable-v1.xml \
	> $@/zwp-relative-pointer-protocol.c
	@wayland-scanner client-header \
	< /usr/share/wayland-protocols/unstable/relative-pointer/relative-pointer-unstable-v1.xml \
	> $@/zwp-relative-pointer-protocol.h
endif

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

## windows icon
$(OBJD)/$(RESD)/icon/iconpix.obj: $(RESD)/icon/iconpix.bin
	@echo "building icon object"
	@mkdir -p $(@D)
	@objcopy -I binary -O pe-x86-64 -B i386:x86-64 \
	--redefine-syms=$(RESD)/icon/syms.map \
	--rename-section .data=.iconpix \
	$< $@

## macOS icon
$(RESD)/objconv/objconv:
	@echo "making objconv"
	@cd ./$(RESD)/objconv && ./makeobjconv.sh

$(OBJD)/$(RESD)/icon/iconpix_mach.o: $(RESD)/icon/iconpix.bin $(RESD)/objconv/objconv
	@echo "building icon object object and converting to mach-o (bug workaround)"
	@mkdir -p $(@D)
	@$(OBJCOPY) \
	-I binary \
	-O elf64-x86-64 \
	-B i386:x86-64 \
	--redefine-syms=$(RESD)/icon/syms.map \
	--rename-section .data=.iconpix \
	$< $(OBJD)/$(RESD)/icon/iconpix.o
	@./$(RESD)/objconv/objconv -fmac64 -nu+ -v0 $(OBJD)/$(RESD)/icon/iconpix.o $@

## macOS ANGLE
$(RESD)/angle/libs:
	@echo "getting ANGLE"
	@cd ./$(RESD)/angle && ./getmetalangle.sh

## compilation with gcc for X11, Wayland and Windows
$(OBJD)/%.o: %.c
	@echo "building object $@"
	@mkdir -p $(@D)
	@$(CC) $(INCL) $(FLAGS) -c -o $@ $<

$(BIND)/$(NAME): $(SRCS_OBJS)
	@echo "compiling executable $@"
	@mkdir -p $(@D)
	@$(CC) -o $@ $^ $(LINK)

$(BIND)/$(NAME).exe: $(SRCS_OBJS)
	@echo "compiling executable $@"
	@mkdir -p $(@D)
	@$(CC) -o $@ $^ $(LINK)

## compilation with MSVC for Windows
$(OBJD)/%.obj: %.c
	@echo "building object $@"
	@mkdir -p $(@D)
	@$(CC) $(INCL) $(FLAGS) -Fo$@ -c $<

$(BIND)/$(NAME)_msvc.exe: $(SRCS_OBJS)
	@echo "compiling executable $@"
	@mkdir -p $(@D)
	@$(CC) -Fe$@ $(LINK) $^ -link $(LINK_WINDOWS)

## compilation with clang for macOS
ifeq ($(CONTEXT), EGL)
$(BIND)/$(NAME).app: $(RESD)/angle/libs $(BIND)/$(NAME)
	@echo "renaming binary to $@"
	@cp $(RESD)/angle/libs/*.dylib $(BIND)/
	@mv $(BIND)/$(NAME) $(BIND)/$(NAME).app
else
$(BIND)/$(NAME).app: $(BIND)/$(NAME)
	@echo "renaming binary to $@"
	@mv $^ $@
endif

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
