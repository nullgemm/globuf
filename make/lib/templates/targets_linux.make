$(FOLDER)/$(NAME).a: $(OBJ)
	mkdir -p $(@D)
	ar rcs $@ $^

res/wayland_headers:
	make/scripts/wayland_get.sh
