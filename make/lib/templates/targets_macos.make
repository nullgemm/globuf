bin/lib$(NAME).dylib: lib$(NAME).dylib
	mkdir -p $(@D)
	mv $^ $@

lib$(NAME).dylib: $(OBJ)
	mkdir -p $(@D)
	$(CC) $(LDFLAGS) -shared -o $@ $^ $(LDLIBS)

bin/$(NAME).a: $(OBJ)
	mkdir -p $(@D)
	$(AR) rcs $@ $^

res/angle/libs:
	make/scripts/angle_dev_get.sh
