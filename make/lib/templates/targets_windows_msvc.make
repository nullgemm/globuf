bin/$(NAME).dll: $(OBJ)
	mkdir -p $(@D)
	$(CC) -D_USRDLL -D_WINDLL $^ -link $(LDFLAGS) $(LDLIBS) -DLL -OUT:bin/$(NAME).dll

bin/$(NAME).lib: $(OBJ)
	mkdir -p $(@D)
	$(LIB) -OUT:$@ $^

res/egl_headers:
	make/scripts/egl_get.sh

res/eglproxy:
	make/scripts/eglproxy_get.sh

.SUFFIXES: .c .obj
.c.obj:
	$(CC) $(CFLAGS) -Fo$@ -c $<
