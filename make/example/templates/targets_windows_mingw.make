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

run: bin/$(NAME)
	cd bin && $(CMD)
