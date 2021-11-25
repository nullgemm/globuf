bin/$(NAME): $(OBJ) $(OBJ_EXTRA)
	mkdir -p $(@D)
	$(CC) -Febin/$(NAME).exe $^ -link -ENTRY:mainCRTStartup $(LDFLAGS) $(LDLIBS)

bin/eglproxy.dll: res/eglproxy
	mkdir -p $(@D)
	cp res/eglproxy/lib/msvc/eglproxy.dll $@

res/eglproxy:
	make/scripts/eglproxy_get.sh

res/egl_headers:
	make/scripts/egl_get.sh

res/icon/iconpix.bin:
	make/scripts/pixmap_bin.sh

res/icon/iconpix_pe.obj: res/icon/iconpix.bin
	objcopy -I binary -O pe-x86-64 -B i386:x86-64 \
	--redefine-syms=res/icon/syms.map \
	--rename-section .data=.iconpix \
	$< $@

run: bin/$(NAME)
	cd bin && $(CMD)

leak: bin/$(NAME).exe
	cd bin && drmemory.exe $(DRMEMORY) 2> ../drmemory.log $(CMD)
	less drmemory.log

.SUFFIXES: .c .obj
.c.obj:
	$(CC) $(CFLAGS) -Fo$@ -c $<
