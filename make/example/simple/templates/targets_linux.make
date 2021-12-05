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

res/shaders/gl1/square_vert_gl1.o: res/shaders/gl1/square_vert_gl1.glsl
	objcopy -I binary -O elf64-x86-64 -B i386:x86-64 \
	--redefine-syms=res/shaders/gl1/syms.map \
	--rename-section .data=.square_vert \
	$< $@

res/shaders/gl1/square_frag_gl1.o: res/shaders/gl1/square_frag_gl1.glsl
	objcopy -I binary -O elf64-x86-64 -B i386:x86-64 \
	--redefine-syms=res/shaders/gl1/syms.map \
	--rename-section .data=.square_frag \
	$< $@

leak: bin/$(NAME)
	cd bin && valgrind $(VALGRIND) 2> ../valgrind.log $(CMD)
	less valgrind.log

run: bin/$(NAME)
	cd bin && $(CMD)
