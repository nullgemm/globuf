$(FOLDER_INCLUDE)/globox.h:
$(FOLDER_INCLUDE)/globox_$(BACKEND).h:
$(FOLDER_INCLUDE)/globox_$(PLATFORM)_$(BACKEND).h:
	make/scripts/release_headers.sh $@

headers: \
$(FOLDER_INCLUDE)/globox.h \
$(FOLDER_INCLUDE)/globox_$(BACKEND).h \
$(FOLDER_INCLUDE)/globox_$(PLATFORM)_$(BACKEND).h

clean:
	make/scripts/clean.sh
