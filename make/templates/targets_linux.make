bin/$(NAME): $(OBJ)
	mkdir -p $(@D)
	$(CC) $(LDFLAGS) -o $@ $^ $(LDLIBS)

res/icon/iconpix.bin:
	make/pixmap_bin.sh

res/icon/iconpix.o: res/icon/iconpix.bin
	objcopy -I binary -O elf64-x86-64 -B i386:x86-64 \
	--redefine-syms=res/icon/syms.map \
	--rename-section .data=.iconpix \
	$< $@

leak: bin/$(NAME)
	@echo "# running valgrind"
	cd bin && valgrind $(VALGRIND) 2> ../valgrind.log $(CMD)
	less valgrind.log
