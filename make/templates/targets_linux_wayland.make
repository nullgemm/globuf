final: res/wayland_headers bin/$(NAME)

res/wayland_headers:
	make/scripts/wayland_get.sh
