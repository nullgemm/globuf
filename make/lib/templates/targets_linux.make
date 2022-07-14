$(FOLDER)/$(NAME).so: $(OBJ)
	mkdir -p $(@D)
	$(CC) $(LDFLAGS) -shared -o $@ $^ $(LDLIBS)

$(FOLDER)/$(NAME).a: $(OBJ)
	mkdir -p $(@D)
	ar rcs $@ $^

res/wayland_headers:
	make/scripts/wayland_get.sh
