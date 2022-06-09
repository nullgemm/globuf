versions:
	@git tag --sort v:refname

clean_makefiles:
	make/scripts/clean_makefiles.sh

# regular makefile targets for release library builds
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

# named makefile targets for development library builds
development_makefile_lib_x11_glx:
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

# named makefile targets for sanitized library builds
sanitized_makefile_lib_macos_software:
	make/lib/gen/gen_macos.sh release software osxcross osxcross

sanitized_makefile_lib_macos_software_native:
	make/lib/gen/gen_macos.sh release software native native

sanitized_makefile_lib_wayland_software:
	make/lib/gen/gen_wayland.sh release software

sanitized_makefile_lib_x11_software:
	make/lib/gen/gen_x11.sh release software

sanitized_makefile_lib_macos_egl:
	make/lib/gen/gen_macos.sh release egl osxcross osxcross

sanitized_makefile_lib_macos_egl_native:
	make/lib/gen/gen_macos.sh release egl native native

sanitized_makefile_lib_wayland_egl:
	make/lib/gen/gen_wayland.sh release egl

sanitized_makefile_lib_x11_egl:
	make/lib/gen/gen_x11.sh release egl

sanitized_makefile_lib_x11_glx:
	make/lib/gen/gen_x11.sh release glx

# targets for software library binaries
bin/globox_macos_software: \
make/output/makefile_lib_macos_software
	make/scripts/run_osxcross.sh make/output/makefile_lib_macos_software

bin/globox_macos_software_native: \
make/output/makefile_lib_macos_software_native
	$(MAKE) -f make/output/makefile_lib_macos_software_native

bin/globox_windows_software: \
make/output/makefile_lib_windows_software
	$(MAKE) -f make/output/makefile_lib_windows_software

bin/globox_windows_software_native: \
make/output/makefile_lib_windows_software_native
	$(MAKE) -f make/output/makefile_lib_windows_software_native

bin/globox_wayland_software: \
make/output/makefile_lib_wayland_software
	$(MAKE) -f make/output/makefile_lib_wayland_software

bin/globox_x11_software: \
make/output/makefile_lib_x11_software
	$(MAKE) -f make/output/makefile_lib_x11_software

# targets for egl library binaries
bin/globox_macos_egl: \
make/output/makefile_lib_macos_egl
	make/scripts/run_osxcross.sh make/output/makefile_lib_macos_egl

bin/globox_macos_egl_native: \
make/output/makefile_lib_macos_egl_native
	$(MAKE) -f make/output/makefile_lib_macos_egl_native

bin/globox_windows_egl: \
make/output/makefile_lib_windows_egl
	$(MAKE) -f make/output/makefile_lib_windows_egl

bin/globox_windows_egl_native: \
make/output/makefile_lib_windows_egl_native
	$(MAKE) -f make/output/makefile_lib_windows_egl_native

bin/globox_wayland_egl: \
make/output/makefile_lib_wayland_egl
	$(MAKE) -f make/output/makefile_lib_wayland_egl

bin/globox_x11_egl: \
make/output/makefile_lib_x11_egl
	$(MAKE) -f make/output/makefile_lib_x11_egl

# targets for platform-specific gl-binder library binaries
bin/globox_windows_wgl: \
make/output/makefile_lib_windows_wgl
	$(MAKE) -f make/output/makefile_lib_windows_wgl

bin/globox_windows_wgl_native: \
make/output/makefile_lib_windows_wgl_native
	$(MAKE) -f make/output/makefile_lib_windows_wgl_native

bin/globox_x11_glx: \
make/output/makefile_lib_x11_glx
	$(MAKE) -f make/output/makefile_lib_x11_glx
