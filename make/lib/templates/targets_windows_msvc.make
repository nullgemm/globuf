$(FOLDER)/$(NAME).lib: $(OBJ)
	mkdir -p $(@D)
	$(LIB) -OUT:$@ $^

res/egl_headers:
	make/scripts/egl_get.sh

res/eglproxy:
	make/scripts/eglproxy_get.sh

.SUFFIXES: .c .obj
.c.obj:
	$(CC) $(CFLAGS) -Fo$@ -c $<
