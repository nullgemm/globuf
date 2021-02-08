bin/$(NAME).dll: $(OBJ)
	mkdir -p $(@D)
	$(CC) $(LDFLAGS) -shared -o $@ $^ $(LDLIBS)

bin/$(NAME).a: $(OBJ)
	mkdir -p $(@D)
	ar rcs $@ $^
