bin/$(NAME).exe: $(OBJ) $(EXAMPLE)
	mkdir -p $(@D)
	$(CC) -Febin/$(NAME).exe $^ -link -ENTRY:mainCRTStartup $(LDFLAGS) $(LDLIBS)

res/icon/iconpix.bin:
	make/scripts/pixmap_bin.sh

res/icon/iconpix_pe.obj: res/icon/iconpix.bin
	objcopy -I binary -O pe-x86-64 -B i386:x86-64 \
	--redefine-syms=res/icon/syms.map \
	--rename-section .data=.iconpix \
	$< $@

leak: bin/$(NAME).exe
	@echo "# running Dr.Memory"
	cd bin && drmemory.exe $(DRMEMORY) 2> ../drmemory.log $(CMD)
	less drmemory.log
