final: res/egl_headers bin/eglproxy.dll bin/$(NAME).exe

bin/eglproxy.dll: res/eglproxy/bin/eglproxy.dll
	mkdir -p $(@D)
	cp res/eglproxy/bin/eglproxy.dll $@

res/eglproxy/bin/eglproxy.dll: res/eglproxy/readme.md
	cd res/eglproxy && NATIVE=FALSE make
	rm res/eglproxy/bin/eglproxy.lib

res/egl_headers:
	make/scripts/egl_get.sh
