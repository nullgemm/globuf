#!/bin/bash

ar -x globuf_appkit_common_native.a
ar -x globuf_appkit_software_native.a
ar -x ../globuf_macho_native.a
clang -shared -o globuf_appkit_software_native.dylib *.o -framework AppKit -framework QuartzCore
