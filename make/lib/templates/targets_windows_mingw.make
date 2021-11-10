bin/$(NAME).dll: $(OBJ)
	mkdir -p $(@D)
	$(CC) $(LDFLAGS) -shared -o $@ $^ $(LDLIBS) -Wl,--out-implib,$(@D)/$(NAME).dll.a

bin/$(NAME).a: $(OBJ)
	mkdir -p $(@D)
	$(AR) rcs $@ $^

res/egl_headers:
	make/scripts/egl_get.sh

res/eglproxy:
	make/scripts/eglproxy_get.sh
