bin/$(NAME): $(OBJ) $(OBJ_EXTRA)
	mkdir -p $(@D)
	$(CC) $(LDFLAGS) -o $@ $^ $(LDLIBS)

bin/eglproxy.dll: res/eglproxy
	mkdir -p $(@D)
	cp res/eglproxy/lib/mingw/eglproxy.dll $@

res/eglproxy:
	make/scripts/eglproxy_get.sh

res/egl_headers:
	make/scripts/egl_get.sh

res/icon/iconpix.bin:
	make/scripts/pixmap_bin.sh

res/icon/iconpix_pe.o: res/icon/iconpix.bin
	objcopy -I binary -O pe-x86-64 -B i386:x86-64 \
	--redefine-syms=res/icon/syms.map \
	--rename-section .data=.iconpix \
	$< $@

res/shaders/gl1/square_vert_gl1_pe.o: res/shaders/gl1/square_vert_gl1.glsl
	objcopy -I binary -O pe-x86-64 -B i386:x86-64 \
	--redefine-syms=res/shaders/gl1/syms.map \
	--rename-section .data=.square_vert \
	$< $@

res/shaders/gl1/square_frag_gl1_pe.o: res/shaders/gl1/square_frag_gl1.glsl
	objcopy -I binary -O pe-x86-64 -B i386:x86-64 \
	--redefine-syms=res/shaders/gl1/syms.map \
	--rename-section .data=.square_frag \
	$< $@

res/shaders/gl1/decorations_vert_gl1_pe.o: res/shaders/gl1/decorations_vert_gl1.glsl
	objcopy -I binary -O pe-x86-64 -B i386:x86-64 \
	--redefine-syms=res/shaders/gl1/syms.map \
	--rename-section .data=.decorations_vert \
	$< $@

res/shaders/gl1/decorations_frag_gl1_pe.o: res/shaders/gl1/decorations_frag_gl1.glsl
	objcopy -I binary -O pe-x86-64 -B i386:x86-64 \
	--redefine-syms=res/shaders/gl1/syms.map \
	--rename-section .data=.decorations_frag \
	$< $@

run: bin/$(NAME)
	cd bin && $(CMD)
