$(FOLDER_INCLUDE)/globox.h:
$(FOLDER_INCLUDE)/globox_private.h:
	make/scripts/release_headers.sh

headers: \
$(FOLDER_INCLUDE)/globox.h \
$(FOLDER_INCLUDE)/globox_private.h

clean:
	make/scripts/clean.sh
