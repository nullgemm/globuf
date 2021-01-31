.SUFFIXES: .c .obj
.c.obj:
	$(CC) $(CFLAGS) -Fo$@ -c $<
