final: bin/eglproxy.dll bin/$(NAME).exe

bin/$(NAME).exe: $(OBJ)
	mkdir -p $(@D)
	$(CC) $(LDFLAGS) -o bin/$(NAME) $^ $(LDLIBS)

res/icon/iconpix.bin:
	make/pixmap_bin.sh

res/icon/iconpix_pe.o: res/icon/iconpix.bin
	objcopy -I binary -O pe-x86-64 -B i386:x86-64 \
	--redefine-syms=res/icon/syms.map \
	--rename-section .data=.iconpix \
	$< $@

bin/eglproxy.dll: res/eglproxy/bin/eglproxy.dll
	mkdir -p $(@D)
	cp res/eglproxy/bin/eglproxy.dll $@

res/eglproxy/bin/eglproxy.dll: res/eglproxy/readme.md
	cd res/eglproxy && NATIVE=FALSE make
	rm res/eglproxy/bin/eglproxy.lib
