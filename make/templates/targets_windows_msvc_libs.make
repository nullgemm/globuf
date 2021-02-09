bin/$(NAME).dll: $(OBJ)
	mkdir -p $(@D)
	$(CC) -D_USRDLL -D_WINDLL $^ -link $(LDFLAGS) $(LDLIBS) -DLL -OUT:bin/$(NAME).dll

bin/$(NAME).lib: $(OBJ)
	mkdir -p $(@D)
	$(LIB) -OUT:$@ $^
