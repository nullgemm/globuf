bin/$(NAME): $(OBJ) $(OBJ_EXTRA)
	mkdir -p $(@D)
	$(CC) $(LDFLAGS) -o $@ $^ $(LDLIBS)

res/icon/iconpix.bin:
	make/scripts/pixmap_bin.sh

res/icon/iconpix.o: res/icon/iconpix.bin
	objcopy -I binary -O elf64-x86-64 -B i386:x86-64 \
	--redefine-syms=res/icon/syms.map \
	--rename-section .data=.iconpix \
	$< $@

res/wayland_headers:
	make/scripts/wayland_get.sh

leak: bin/$(NAME)
	cd bin && valgrind $(VALGRIND) 2> ../valgrind.log $(CMD)
	less valgrind.log

run: bin/$(NAME)
	cd bin && $(CMD)
