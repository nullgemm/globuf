# utils
versions:
	@git tag --sort v:refname

clean:
	make/scripts/clean.sh

clean_makefiles:
	make/scripts/clean_makefiles.sh

# named & regular makefile targets for release library builds
release_makefile_lib_macos_software:
make/output/makefile_lib_macos_software:
	make/lib/gen/gen_macos.sh release software osxcross osxcross

release_makefile_lib_macos_software_native:
make/output/makefile_lib_macos_software_native:
	make/lib/gen/gen_macos.sh release software native native

release_makefile_lib_windows_software:
make/output/makefile_lib_windows_software:
	make/lib/gen/gen_windows_mingw.sh release software

release_makefile_lib_windows_software_native:
make/output/makefile_lib_windows_software_native:
	make/lib/gen/gen_windows_msvc.sh release software

release_makefile_lib_wayland_software:
make/output/makefile_lib_wayland_software:
	make/lib/gen/gen_wayland.sh release software

release_makefile_lib_x11_software:
make/output/makefile_lib_x11_software:
	make/lib/gen/gen_x11.sh release software

release_makefile_lib_macos_egl:
make/output/makefile_lib_macos_egl:
	make/lib/gen/gen_macos.sh release egl osxcross osxcross

release_makefile_lib_macos_egl_native:
make/output/makefile_lib_macos_egl_native:
	make/lib/gen/gen_macos.sh release egl native native

release_makefile_lib_windows_egl:
make/output/makefile_lib_windows_egl:
	make/lib/gen/gen_windows_mingw.sh release egl

release_makefile_lib_windows_egl_native:
make/output/makefile_lib_windows_egl_native:
	make/lib/gen/gen_windows_msvc.sh release egl

release_makefile_lib_wayland_egl:
make/output/makefile_lib_wayland_egl:
	make/lib/gen/gen_wayland.sh release egl

release_makefile_lib_x11_egl:
make/output/makefile_lib_x11_egl:
	make/lib/gen/gen_x11.sh release egl

release_makefile_lib_windows_wgl:
make/output/makefile_lib_windows_wgl:
	make/lib/gen/gen_windows_mingw.sh release wgl

release_makefile_lib_windows_wgl_native:
make/output/makefile_lib_windows_wgl_native:
	make/lib/gen/gen_windows_msvc.sh release wgl

release_makefile_lib_x11_glx:
make/output/makefile_lib_x11_glx:
	make/lib/gen/gen_x11.sh release glx

# named makefile targets for development library builds
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

# targets for egl library binaries
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

# targets for platform-specific gl-binder library binaries
globox_lib_windows_wgl: \
make/output/makefile_lib_windows_wgl
	$(MAKE) -f make/output/makefile_lib_windows_wgl

globox_lib_windows_wgl_native: \
make/output/makefile_lib_windows_wgl_native
	$(MAKE) -f make/output/makefile_lib_windows_wgl_native

globox_lib_x11_glx: \
make/output/makefile_lib_x11_glx
	$(MAKE) -f make/output/makefile_lib_x11_glx
