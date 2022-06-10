# utils
versions:
	@git tag --sort v:refname

clean:
	make/scripts/clean.sh

clean_makefiles:
	make/scripts/clean_makefiles.sh



# aliases
## release library aliases
release_makefile_lib_macos_software: \
make/output/makefile_lib_macos_software
release_makefile_lib_macos_software_native: \
make/output/makefile_lib_macos_software_native
release_makefile_lib_windows_software: \
make/output/makefile_lib_windows_software
release_makefile_lib_windows_software_native: \
make/output/makefile_lib_windows_software_native
release_makefile_lib_wayland_software: \
make/output/makefile_lib_wayland_software
release_makefile_lib_x11_software: \
make/output/makefile_lib_x11_software
release_makefile_lib_macos_egl: \
make/output/makefile_lib_macos_egl
release_makefile_lib_macos_egl_native: \
make/output/makefile_lib_macos_egl_native
release_makefile_lib_windows_egl: \
make/output/makefile_lib_windows_egl
release_makefile_lib_windows_egl_native: \
make/output/makefile_lib_windows_egl_native
release_makefile_lib_wayland_egl: \
make/output/makefile_lib_wayland_egl
release_makefile_lib_x11_egl: \
make/output/makefile_lib_x11_egl
release_makefile_lib_windows_wgl: \
make/output/makefile_lib_windows_wgl
release_makefile_lib_windows_wgl_native: \
make/output/makefile_lib_windows_wgl_native
release_makefile_lib_x11_glx: \
make/output/makefile_lib_x11_glx


## release simple example aliases (static)
release_makefile_example_simple_macos_software_static: \
make/output/makefile_example_simple_macos_software
release_makefile_example_simple_macos_software_native_static: \
make/output/makefile_example_simple_macos_software_native
release_makefile_example_simple_windows_software_static: \
make/output/makefile_example_simple_windows_software
release_makefile_example_simple_windows_software_native_static: \
make/output/makefile_example_simple_windows_software_native
release_makefile_example_simple_wayland_software_static: \
make/output/makefile_example_simple_wayland_software
release_makefile_example_simple_x11_software_static: \
make/output/makefile_example_simple_x11_software
release_makefile_example_simple_macos_egl_static: \
make/output/makefile_example_simple_macos_egl
release_makefile_example_simple_macos_egl_native_static: \
make/output/makefile_example_simple_macos_egl_native
release_makefile_example_simple_windows_egl_static: \
make/output/makefile_example_simple_windows_egl
release_makefile_example_simple_windows_egl_native_static: \
make/output/makefile_example_simple_windows_egl_native
release_makefile_example_simple_wayland_egl_static: \
make/output/makefile_example_simple_wayland_egl
release_makefile_example_simple_x11_egl_static: \
make/output/makefile_example_simple_x11_egl
release_makefile_example_simple_windows_wgl_static: \
make/output/makefile_example_simple_windows_wgl
release_makefile_example_simple_windows_wgl_native_static: \
make/output/makefile_example_simple_windows_wgl_native
release_makefile_example_simple_x11_glx_static: \
make/output/makefile_example_simple_x11_glx


## release complex example aliases (static)
release_makefile_example_complex_macos_software_static: \
make/output/makefile_example_complex_macos_software
release_makefile_example_complex_macos_software_native_static: \
make/output/makefile_example_complex_macos_software_native
release_makefile_example_complex_windows_software_static: \
make/output/makefile_example_complex_windows_software
release_makefile_example_complex_windows_software_native_static: \
make/output/makefile_example_complex_windows_software_native
release_makefile_example_complex_wayland_software_static: \
make/output/makefile_example_complex_wayland_software
release_makefile_example_complex_x11_software_static: \
make/output/makefile_example_complex_x11_software
release_makefile_example_complex_macos_egl_static: \
make/output/makefile_example_complex_macos_egl
release_makefile_example_complex_macos_egl_native_static: \
make/output/makefile_example_complex_macos_egl_native
release_makefile_example_complex_windows_egl_static: \
make/output/makefile_example_complex_windows_egl
release_makefile_example_complex_windows_egl_native_static: \
make/output/makefile_example_complex_windows_egl_native
release_makefile_example_complex_wayland_egl_static: \
make/output/makefile_example_complex_wayland_egl
release_makefile_example_complex_x11_egl_static: \
make/output/makefile_example_complex_x11_egl
release_makefile_example_complex_windows_wgl_static: \
make/output/makefile_example_complex_windows_wgl
release_makefile_example_complex_windows_wgl_native_static: \
make/output/makefile_example_complex_windows_wgl_native
release_makefile_example_complex_x11_glx_static: \
make/output/makefile_example_complex_x11_glx



# library makefiles targets
## named & regular makefile targets for release library builds
make/output/makefile_lib_macos_software:
	make/lib/gen/gen_macos.sh release software osxcross osxcross

make/output/makefile_lib_macos_software_native:
	make/lib/gen/gen_macos.sh release software native native

make/output/makefile_lib_windows_software:
	make/lib/gen/gen_windows_mingw.sh release software

make/output/makefile_lib_windows_software_native:
	make/lib/gen/gen_windows_msvc.sh release software

make/output/makefile_lib_wayland_software:
	make/lib/gen/gen_wayland.sh release software

make/output/makefile_lib_x11_software:
	make/lib/gen/gen_x11.sh release software

make/output/makefile_lib_macos_egl:
	make/lib/gen/gen_macos.sh release egl osxcross osxcross

make/output/makefile_lib_macos_egl_native:
	make/lib/gen/gen_macos.sh release egl native native

make/output/makefile_lib_windows_egl:
	make/lib/gen/gen_windows_mingw.sh release egl

make/output/makefile_lib_windows_egl_native:
	make/lib/gen/gen_windows_msvc.sh release egl

make/output/makefile_lib_wayland_egl:
	make/lib/gen/gen_wayland.sh release egl

make/output/makefile_lib_x11_egl:
	make/lib/gen/gen_x11.sh release egl

make/output/makefile_lib_windows_wgl:
	make/lib/gen/gen_windows_mingw.sh release wgl

make/output/makefile_lib_windows_wgl_native:
	make/lib/gen/gen_windows_msvc.sh release wgl

make/output/makefile_lib_x11_glx:
	make/lib/gen/gen_x11.sh release glx


## named makefile targets for development library builds
development_makefile_lib_macos_software:
	make/lib/gen/gen_macos.sh development software osxcross osxcross

development_makefile_lib_macos_software_native:
	make/lib/gen/gen_macos.sh development software native native

development_makefile_lib_windows_software:
	make/lib/gen/gen_windows_mingw.sh development software

development_makefile_lib_windows_software_native:
	make/lib/gen/gen_windows_msvc.sh development software

development_makefile_lib_wayland_software:
	make/lib/gen/gen_wayland.sh development software

development_makefile_lib_x11_software:
	make/lib/gen/gen_x11.sh development software

development_makefile_lib_macos_egl:
	make/lib/gen/gen_macos.sh development egl osxcross osxcross

development_makefile_lib_macos_egl_native:
	make/lib/gen/gen_macos.sh development egl native native

development_makefile_lib_windows_egl:
	make/lib/gen/gen_windows_mingw.sh development egl

development_makefile_lib_windows_egl_native:
	make/lib/gen/gen_windows_msvc.sh development egl

development_makefile_lib_wayland_egl:
	make/lib/gen/gen_wayland.sh development egl

development_makefile_lib_x11_egl:
	make/lib/gen/gen_x11.sh development egl

development_makefile_lib_windows_wgl:
	make/lib/gen/gen_windows_mingw.sh development wgl

development_makefile_lib_windows_wgl_native:
	make/lib/gen/gen_windows_msvc.sh development wgl

development_makefile_lib_x11_glx:
	make/lib/gen/gen_x11.sh development glx


## named makefile targets for sanitized library builds
sanitized_makefile_lib_macos_software:
	make/lib/gen/gen_macos.sh sanitized software osxcross osxcross

sanitized_makefile_lib_macos_software_native:
	make/lib/gen/gen_macos.sh sanitized software native native

sanitized_makefile_lib_wayland_software:
	make/lib/gen/gen_wayland.sh sanitized software

sanitized_makefile_lib_x11_software:
	make/lib/gen/gen_x11.sh sanitized software

sanitized_makefile_lib_macos_egl:
	make/lib/gen/gen_macos.sh sanitized egl osxcross osxcross

sanitized_makefile_lib_macos_egl_native:
	make/lib/gen/gen_macos.sh sanitized egl native native

sanitized_makefile_lib_wayland_egl:
	make/lib/gen/gen_wayland.sh sanitized egl

sanitized_makefile_lib_x11_egl:
	make/lib/gen/gen_x11.sh sanitized egl

sanitized_makefile_lib_x11_glx:
	make/lib/gen/gen_x11.sh sanitized glx



# simple example makefiles targets (static)
## named & regular makefile targets for release simple example builds (static)
make/output/makefile_example_simple_macos_software:
	make/example/simple/gen/gen_macos.sh release software osxcross osxcross static

make/output/makefile_example_simple_macos_software_native:
	make/example/simple/gen/gen_macos.sh release software native native static

make/output/makefile_example_simple_windows_software:
	make/example/simple/gen/gen_windows_mingw.sh release software static

make/output/makefile_example_simple_windows_software_native:
	make/example/simple/gen/gen_windows_msvc.sh release software static

make/output/makefile_example_simple_wayland_software:
	make/example/simple/gen/gen_wayland.sh release software static

make/output/makefile_example_simple_x11_software:
	make/example/simple/gen/gen_x11.sh release software static

make/output/makefile_example_simple_macos_egl:
	make/example/simple/gen/gen_macos.sh release egl osxcross osxcross static

make/output/makefile_example_simple_macos_egl_native:
	make/example/simple/gen/gen_macos.sh release egl native native static

make/output/makefile_example_simple_windows_egl:
	make/example/simple/gen/gen_windows_mingw.sh release egl static

make/output/makefile_example_simple_windows_egl_native:
	make/example/simple/gen/gen_windows_msvc.sh release egl static

make/output/makefile_example_simple_wayland_egl:
	make/example/simple/gen/gen_wayland.sh release egl static

make/output/makefile_example_simple_x11_egl:
	make/example/simple/gen/gen_x11.sh release egl static

make/output/makefile_example_simple_windows_wgl:
	make/example/simple/gen/gen_windows_mingw.sh release wgl static

make/output/makefile_example_simple_windows_wgl_native:
	make/example/simple/gen/gen_windows_msvc.sh release wgl static

make/output/makefile_example_simple_x11_glx:
	make/example/simple/gen/gen_x11.sh release glx static


## named makefile targets for development simple example builds (static)
development_makefile_example_simple_macos_software_static:
	make/example/simple/gen/gen_macos.sh development software osxcross osxcross static

development_makefile_example_simple_macos_software_native_static:
	make/example/simple/gen/gen_macos.sh development software native native static

development_makefile_example_simple_windows_software_static:
	make/example/simple/gen/gen_windows_mingw.sh development software static

development_makefile_example_simple_windows_software_native_static:
	make/example/simple/gen/gen_windows_msvc.sh development software static

development_makefile_example_simple_wayland_software_static:
	make/example/simple/gen/gen_wayland.sh development software static

development_makefile_example_simple_x11_software_static:
	make/example/simple/gen/gen_x11.sh development software static

development_makefile_example_simple_macos_egl_static:
	make/example/simple/gen/gen_macos.sh development egl osxcross osxcross static

development_makefile_example_simple_macos_egl_native_static:
	make/example/simple/gen/gen_macos.sh development egl native native static

development_makefile_example_simple_windows_egl_static:
	make/example/simple/gen/gen_windows_mingw.sh development egl static

development_makefile_example_simple_windows_egl_native_static:
	make/example/simple/gen/gen_windows_msvc.sh development egl static

development_makefile_example_simple_wayland_egl_static:
	make/example/simple/gen/gen_wayland.sh development egl static

development_makefile_example_simple_x11_egl_static:
	make/example/simple/gen/gen_x11.sh development egl static

development_makefile_example_simple_windows_wgl_static:
	make/example/simple/gen/gen_windows_mingw.sh development wgl static

development_makefile_example_simple_windows_wgl_native_static:
	make/example/simple/gen/gen_windows_msvc.sh development wgl static

development_makefile_example_simple_x11_glx_static:
	make/example/simple/gen/gen_x11.sh development glx static


## named makefile targets for sanitized simple example builds (static)
sanitized_makefile_example_simple_macos_software_static:
	make/example/simple/gen/gen_macos.sh sanitized software osxcross osxcross static

sanitized_makefile_example_simple_macos_software_native_static:
	make/example/simple/gen/gen_macos.sh sanitized software native native static

sanitized_makefile_example_simple_wayland_software_static:
	make/example/simple/gen/gen_wayland.sh sanitized software static

sanitized_makefile_example_simple_x11_software_static:
	make/example/simple/gen/gen_x11.sh sanitized software static

sanitized_makefile_example_simple_macos_egl_static:
	make/example/simple/gen/gen_macos.sh sanitized egl osxcross osxcross static

sanitized_makefile_example_simple_macos_egl_native_static:
	make/example/simple/gen/gen_macos.sh sanitized egl native native static

sanitized_makefile_example_simple_wayland_egl_static:
	make/example/simple/gen/gen_wayland.sh sanitized egl static

sanitized_makefile_example_simple_x11_egl_static:
	make/example/simple/gen/gen_x11.sh sanitized egl static

sanitized_makefile_example_simple_x11_glx_static:
	make/example/simple/gen/gen_x11.sh sanitized glx static



# simple example makefiles targets (shared)
## named & regular makefile targets for release simple example builds (shared)
make/output/makefile_example_simple_macos_software_shared:
	make/example/simple/gen/gen_macos.sh release software osxcross osxcross shared

make/output/makefile_example_simple_macos_software_native_shared:
	make/example/simple/gen/gen_macos.sh release software native native shared

make/output/makefile_example_simple_windows_software_shared:
	make/example/simple/gen/gen_windows_mingw.sh release software shared

make/output/makefile_example_simple_windows_software_native_shared:
	make/example/simple/gen/gen_windows_msvc.sh release software shared

make/output/makefile_example_simple_wayland_software_shared:
	make/example/simple/gen/gen_wayland.sh release software shared

make/output/makefile_example_simple_x11_software_shared:
	make/example/simple/gen/gen_x11.sh release software shared

make/output/makefile_example_simple_macos_egl_shared:
	make/example/simple/gen/gen_macos.sh release egl osxcross osxcross shared

make/output/makefile_example_simple_macos_egl_native_shared:
	make/example/simple/gen/gen_macos.sh release egl native native shared

make/output/makefile_example_simple_windows_egl_shared:
	make/example/simple/gen/gen_windows_mingw.sh release egl shared

make/output/makefile_example_simple_windows_egl_native_shared:
	make/example/simple/gen/gen_windows_msvc.sh release egl shared

make/output/makefile_example_simple_wayland_egl_shared:
	make/example/simple/gen/gen_wayland.sh release egl shared

make/output/makefile_example_simple_x11_egl_shared:
	make/example/simple/gen/gen_x11.sh release egl shared

make/output/makefile_example_simple_windows_wgl_shared:
	make/example/simple/gen/gen_windows_mingw.sh release wgl shared

make/output/makefile_example_simple_windows_wgl_native_shared:
	make/example/simple/gen/gen_windows_msvc.sh release wgl shared

make/output/makefile_example_simple_x11_glx_shared:
	make/example/simple/gen/gen_x11.sh release glx shared


## named makefile targets for development simple example builds (shared)
development_makefile_example_simple_macos_software_shared:
	make/example/simple/gen/gen_macos.sh development software osxcross osxcross shared

development_makefile_example_simple_macos_software_native_shared:
	make/example/simple/gen/gen_macos.sh development software native native shared

development_makefile_example_simple_windows_software_shared:
	make/example/simple/gen/gen_windows_mingw.sh development software shared

development_makefile_example_simple_windows_software_native_shared:
	make/example/simple/gen/gen_windows_msvc.sh development software shared

development_makefile_example_simple_wayland_software_shared:
	make/example/simple/gen/gen_wayland.sh development software shared

development_makefile_example_simple_x11_software_shared:
	make/example/simple/gen/gen_x11.sh development software shared

development_makefile_example_simple_macos_egl_shared:
	make/example/simple/gen/gen_macos.sh development egl osxcross osxcross shared

development_makefile_example_simple_macos_egl_native_shared:
	make/example/simple/gen/gen_macos.sh development egl native native shared

development_makefile_example_simple_windows_egl_shared:
	make/example/simple/gen/gen_windows_mingw.sh development egl shared

development_makefile_example_simple_windows_egl_native_shared:
	make/example/simple/gen/gen_windows_msvc.sh development egl shared

development_makefile_example_simple_wayland_egl_shared:
	make/example/simple/gen/gen_wayland.sh development egl shared

development_makefile_example_simple_x11_egl_shared:
	make/example/simple/gen/gen_x11.sh development egl shared

development_makefile_example_simple_windows_wgl_shared:
	make/example/simple/gen/gen_windows_mingw.sh development wgl shared

development_makefile_example_simple_windows_wgl_native_shared:
	make/example/simple/gen/gen_windows_msvc.sh development wgl shared

development_makefile_example_simple_x11_glx_shared:
	make/example/simple/gen/gen_x11.sh development glx shared


## named makefile targets for sanitized simple example builds (shared)
sanitized_makefile_example_simple_macos_software_shared:
	make/example/simple/gen/gen_macos.sh sanitized software osxcross osxcross shared

sanitized_makefile_example_simple_macos_software_native_shared:
	make/example/simple/gen/gen_macos.sh sanitized software native native shared

sanitized_makefile_example_simple_wayland_software_shared:
	make/example/simple/gen/gen_wayland.sh sanitized software shared

sanitized_makefile_example_simple_x11_software_shared:
	make/example/simple/gen/gen_x11.sh sanitized software shared

sanitized_makefile_example_simple_macos_egl_shared:
	make/example/simple/gen/gen_macos.sh sanitized egl osxcross osxcross shared

sanitized_makefile_example_simple_macos_egl_native_shared:
	make/example/simple/gen/gen_macos.sh sanitized egl native native shared

sanitized_makefile_example_simple_wayland_egl_shared:
	make/example/simple/gen/gen_wayland.sh sanitized egl shared

sanitized_makefile_example_simple_x11_egl_shared:
	make/example/simple/gen/gen_x11.sh sanitized egl shared

sanitized_makefile_example_simple_x11_glx_shared:
	make/example/simple/gen/gen_x11.sh sanitized glx shared



# library binaries targets
## targets for software library binaries
globox_lib_macos_software: \
make/output/makefile_lib_macos_software
	make/scripts/run_osxcross.sh make/output/makefile_lib_macos_software

globox_lib_macos_software_native: \
make/output/makefile_lib_macos_software_native
	$(MAKE) -f make/output/makefile_lib_macos_software_native

globox_lib_windows_software: \
make/output/makefile_lib_windows_software
	$(MAKE) -f make/output/makefile_lib_windows_software

globox_lib_windows_software_native: \
make/output/makefile_lib_windows_software_native
	$(MAKE) -f make/output/makefile_lib_windows_software_native

globox_lib_wayland_software: \
make/output/makefile_lib_wayland_software
	$(MAKE) -f make/output/makefile_lib_wayland_software

globox_lib_x11_software: \
make/output/makefile_lib_x11_software
	$(MAKE) -f make/output/makefile_lib_x11_software


## targets for egl library binaries
globox_lib_macos_egl: \
make/output/makefile_lib_macos_egl
	make/scripts/run_osxcross.sh make/output/makefile_lib_macos_egl

globox_lib_macos_egl_native: \
make/output/makefile_lib_macos_egl_native
	$(MAKE) -f make/output/makefile_lib_macos_egl_native

globox_lib_windows_egl: \
make/output/makefile_lib_windows_egl
	$(MAKE) -f make/output/makefile_lib_windows_egl

globox_lib_windows_egl_native: \
make/output/makefile_lib_windows_egl_native
	$(MAKE) -f make/output/makefile_lib_windows_egl_native

globox_lib_wayland_egl: \
make/output/makefile_lib_wayland_egl
	$(MAKE) -f make/output/makefile_lib_wayland_egl

globox_lib_x11_egl: \
make/output/makefile_lib_x11_egl
	$(MAKE) -f make/output/makefile_lib_x11_egl

## targets for platform-specific gl-binder library binaries
globox_lib_windows_wgl: \
make/output/makefile_lib_windows_wgl
	$(MAKE) -f make/output/makefile_lib_windows_wgl

globox_lib_windows_wgl_native: \
make/output/makefile_lib_windows_wgl_native
	$(MAKE) -f make/output/makefile_lib_windows_wgl_native

globox_lib_x11_glx: \
make/output/makefile_lib_x11_glx
	$(MAKE) -f make/output/makefile_lib_x11_glx



# simple example binaries targets
## targets for software simple example binaries
globox_simple_example_macos_software_static: \
make/output/makefile_example_simple_macos_software
	$(MAKE) -f make/output/makefile_example_simple_macos_software

globox_simple_example_macos_software_native_static: \
make/output/makefile_example_simple_macos_software_native
	$(MAKE) -f make/output/makefile_example_simple_macos_software_native

globox_simple_example_windows_software_static: \
make/output/makefile_example_simple_windows_software
	$(MAKE) -f make/output/makefile_example_simple_windows_software

globox_simple_example_windows_software_native_static: \
make/output/makefile_example_simple_windows_software_native
	$(MAKE) -f make/output/makefile_example_simple_windows_software_native

globox_simple_example_wayland_software_static: \
make/output/makefile_example_simple_wayland_software
	$(MAKE) -f make/output/makefile_example_simple_wayland_software

globox_simple_example_x11_software_static: \
make/output/makefile_example_simple_x11_software
	$(MAKE) -f make/output/makefile_example_simple_x11_software


## targets for egl simple example binaries
globox_simple_example_macos_egl_static: \
make/output/makefile_example_simple_macos_egl
	$(MAKE) -f make/output/makefile_example_simple_macos_egl

globox_simple_example_macos_egl_native_static: \
make/output/makefile_example_simple_macos_egl_native
	$(MAKE) -f make/output/makefile_example_simple_macos_egl_native

globox_simple_example_windows_egl_static: \
make/output/makefile_example_simple_windows_egl
	$(MAKE) -f make/output/makefile_example_simple_windows_egl

globox_simple_example_windows_egl_native_static: \
make/output/makefile_example_simple_windows_egl_native
	$(MAKE) -f make/output/makefile_example_simple_windows_egl_native

globox_simple_example_wayland_egl_static: \
make/output/makefile_example_simple_wayland_egl
	$(MAKE) -f make/output/makefile_example_simple_wayland_egl

globox_simple_example_x11_egl_static: \
make/output/makefile_example_simple_x11_egl
	$(MAKE) -f make/output/makefile_example_simple_x11_egl


## targets for platform-specific gl-binder simple example binaries
globox_simple_example_windows_wgl_static: \
make/output/makefile_example_simple_windows_wgl
	$(MAKE) -f make/output/makefile_example_simple_windows_wgl

globox_simple_example_windows_wgl_native_static: \
make/output/makefile_example_simple_windows_wgl_native
	$(MAKE) -f make/output/makefile_example_simple_windows_wgl_native

globox_simple_example_x11_glx_static: \
make/output/makefile_example_simple_x11_glx
	$(MAKE) -f make/output/makefile_example_simple_x11_glx
