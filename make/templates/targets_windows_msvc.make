final: bin/eglproxy.dll bin/$(NAME).exe

bin/$(NAME).exe: $(OBJ)
	mkdir -p $(@D)
	$(CC) -Febin/$(NAME) $^ -link $(LDFLAGS) $(LDLIBS)

res/icon/iconpix.bin:
	make/pixmap_bin.sh

res/icon/iconpix_pe.obj: res/icon/iconpix.bin
	objcopy -I binary -O pe-x86-64 -B i386:x86-64 \
	--redefine-syms=res/icon/syms.map \
	--rename-section .data=.iconpix \
	$< $@

bin/eglproxy.dll: res/eglproxy/bin/eglproxy.lib
	mkdir -p $(@D)
	cp res/eglproxy/bin/eglproxy.dll $@

res/eglproxy/bin/eglproxy.lib: res/eglproxy/readme.md
	cd res/eglproxy && NATIVE=TRUE make
