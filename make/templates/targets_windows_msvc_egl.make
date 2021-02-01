final: res/egl_headers bin/eglproxy.dll bin/$(NAME).exe

bin/eglproxy.dll: res/eglproxy/bin/eglproxy.lib
	mkdir -p $(@D)
	cp res/eglproxy/bin/eglproxy.dll $@

res/eglproxy/bin/eglproxy.lib: res/eglproxy/readme.md
	cd res/eglproxy && NATIVE=TRUE make

res/egl_headers:
	make/scripts/egl_get.sh
