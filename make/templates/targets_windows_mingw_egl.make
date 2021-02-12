final: res/egl_headers bin/eglproxy.dll bin/$(NAME).exe

bin/eglproxy.dll: res/eglproxy
	mkdir -p $(@D)
	cp res/eglproxy/lib/mingw/eglproxy.dll $@

res/eglproxy:
	make/scripts/eglproxy_get.sh

res/egl_headers:
	make/scripts/egl_get.sh
