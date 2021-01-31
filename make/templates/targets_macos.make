bin/$(NAME): $(OBJ)
	mkdir -p $(@D)
	$(CC) $(LDFLAGS) -o $@ $^ $(LDLIBS)

res/objconv/objconv:
	make/objconv_make.sh

res/icon/iconpix.bin:
	make/pixmap_bin.sh

res/icon/iconpix_elf.o: res/icon/iconpix.bin res/objconv/objconv
	$(OBJCOPY) -I binary -O elf64-x86-64 -B i386:x86-64 \
	--redefine-syms=res/icon/syms.map \
	--rename-section .data=.iconpix \
	$< $@

res/icon/iconpix_mach.o: res/icon/iconpix_elf.o
	res/objconv/objconv -fmac64 -nu+ -v0 \
	res/icon/iconpix_elf.o $@

leak: bin/$(NAME)
	@echo "# running valgrind"
	cd bin && valgrind $(VALGRIND) 2> ../valgrind.log $(CMD)
	less valgrind.log
