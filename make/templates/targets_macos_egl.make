bin/$(NAME).app: res/angle/libs bin/$(NAME)
	mv bin/$(NAME) $@
	cp res/angle/libs/*.dylib $(@D)

res/angle/libs:
	make/scripts/angle_dev_get.sh
