# utils
angle_dev_get:
	make/scripts/angle_dev_get.sh

angle_dev_make:
	make/scripts/angle_dev_make.sh

angle_master_get:
	make/scripts/angle_master_get.sh

clean:
	make/scripts/clean.sh

clean_makefiles:
	make/scripts/clean_makefiles.sh

egl_get:
	make/scripts/egl_get.sh

eglproxy_get:
	make/scripts/eglproxy_get.sh

objconv_make:
	make/scripts/objconv_make.sh

pixmap_bin:
	make/scripts/pixmap_bin.sh

release_headers:
	make/scripts/release_headers.sh

release_zip:
	make/scripts/release_zip.sh

wayland_get:
	make/scripts/wayland_get.sh



# aliases
## symlinked makefile
default:
	$(MAKE) -f make/output/makefile

run:
	$(MAKE) -f make/output/makefile run

leak:
	$(MAKE) -f make/output/makefile leak


## symlink the makefiles
alias_lib_macos_software:
	mkdir -p make/output
	make/scripts/makefile_ln.sh makefile_lib_macos_software
alias_lib_macos_software_native:
	mkdir -p make/output
	make/scripts/makefile_ln.sh makefile_lib_macos_software_native
alias_lib_windows_software:
	mkdir -p make/output
	make/scripts/makefile_ln.sh makefile_lib_windows_software
alias_lib_windows_software_native:
	mkdir -p make/output
	make/scripts/makefile_ln.sh makefile_lib_windows_software_native
alias_lib_wayland_software:
	mkdir -p make/output
	make/scripts/makefile_ln.sh makefile_lib_wayland_software
alias_lib_x11_software:
	mkdir -p make/output
	make/scripts/makefile_ln.sh makefile_lib_x11_software
alias_lib_macos_egl:
	mkdir -p make/output
	make/scripts/makefile_ln.sh makefile_lib_macos_egl
alias_lib_macos_egl_native:
	mkdir -p make/output
	make/scripts/makefile_ln.sh makefile_lib_macos_egl_native
alias_lib_windows_egl:
	mkdir -p make/output
	make/scripts/makefile_ln.sh makefile_lib_windows_egl
alias_lib_windows_egl_native:
	mkdir -p make/output
	make/scripts/makefile_ln.sh makefile_lib_windows_egl_native
alias_lib_wayland_egl:
	mkdir -p make/output
	make/scripts/makefile_ln.sh makefile_lib_wayland_egl
alias_lib_x11_egl:
	mkdir -p make/output
	make/scripts/makefile_ln.sh makefile_lib_x11_egl
alias_lib_windows_wgl:
	mkdir -p make/output
	make/scripts/makefile_ln.sh makefile_lib_windows_wgl
alias_lib_windows_wgl_native:
	mkdir -p make/output
	make/scripts/makefile_ln.sh makefile_lib_windows_wgl_native
alias_lib_x11_glx:
	mkdir -p make/output
	make/scripts/makefile_ln.sh makefile_lib_x11_glx
alias_example_simple_macos_software:
	mkdir -p make/output
	make/scripts/makefile_ln.sh makefile_example_simple_macos_software
alias_example_simple_macos_software_native:
	mkdir -p make/output
	make/scripts/makefile_ln.sh makefile_example_simple_macos_software_native
alias_example_simple_windows_software:
	mkdir -p make/output
	make/scripts/makefile_ln.sh makefile_example_simple_windows_software
alias_example_simple_windows_software_native:
	mkdir -p make/output
	make/scripts/makefile_ln.sh makefile_example_simple_windows_software_native
alias_example_simple_wayland_software:
	mkdir -p make/output
	make/scripts/makefile_ln.sh makefile_example_simple_wayland_software
alias_example_simple_x11_software:
	mkdir -p make/output
	make/scripts/makefile_ln.sh makefile_example_simple_x11_software
alias_example_simple_macos_egl:
	mkdir -p make/output
	make/scripts/makefile_ln.sh makefile_example_simple_macos_egl
alias_example_simple_macos_egl_native:
	mkdir -p make/output
	make/scripts/makefile_ln.sh makefile_example_simple_macos_egl_native
alias_example_simple_windows_egl:
	mkdir -p make/output
	make/scripts/makefile_ln.sh makefile_example_simple_windows_egl
alias_example_simple_windows_egl_native:
	mkdir -p make/output
	make/scripts/makefile_ln.sh makefile_example_simple_windows_egl_native
alias_example_simple_wayland_egl:
	mkdir -p make/output
	make/scripts/makefile_ln.sh makefile_example_simple_wayland_egl
alias_example_simple_x11_egl:
	mkdir -p make/output
	make/scripts/makefile_ln.sh makefile_example_simple_x11_egl
alias_example_simple_windows_wgl:
	mkdir -p make/output
	make/scripts/makefile_ln.sh makefile_example_simple_windows_wgl
alias_example_simple_windows_wgl_native:
	mkdir -p make/output
	make/scripts/makefile_ln.sh makefile_example_simple_windows_wgl_native
alias_example_simple_x11_glx:
	mkdir -p make/output
	make/scripts/makefile_ln.sh makefile_example_simple_x11_glx
alias_example_complex_macos_software:
	mkdir -p make/output
	make/scripts/makefile_ln.sh makefile_example_complex_macos_software
alias_example_complex_macos_software_native:
	mkdir -p make/output
	make/scripts/makefile_ln.sh makefile_example_complex_macos_software_native
alias_example_complex_windows_software:
	mkdir -p make/output
	make/scripts/makefile_ln.sh makefile_example_complex_windows_software
alias_example_complex_windows_software_native:
	mkdir -p make/output
	make/scripts/makefile_ln.sh makefile_example_complex_windows_software_native
alias_example_complex_wayland_software:
	mkdir -p make/output
	make/scripts/makefile_ln.sh makefile_example_complex_wayland_software
alias_example_complex_x11_software:
	mkdir -p make/output
	make/scripts/makefile_ln.sh makefile_example_complex_x11_software
alias_example_complex_macos_egl:
	mkdir -p make/output
	make/scripts/makefile_ln.sh makefile_example_complex_macos_egl
alias_example_complex_macos_egl_native:
	mkdir -p make/output
	make/scripts/makefile_ln.sh makefile_example_complex_macos_egl_native
alias_example_complex_windows_egl:
	mkdir -p make/output
	make/scripts/makefile_ln.sh makefile_example_complex_windows_egl
alias_example_complex_windows_egl_native:
	mkdir -p make/output
	make/scripts/makefile_ln.sh makefile_example_complex_windows_egl_native
alias_example_complex_wayland_egl:
	mkdir -p make/output
	make/scripts/makefile_ln.sh makefile_example_complex_wayland_egl
alias_example_complex_x11_egl:
	mkdir -p make/output
	make/scripts/makefile_ln.sh makefile_example_complex_x11_egl
alias_example_complex_windows_wgl:
	mkdir -p make/output
	make/scripts/makefile_ln.sh makefile_example_complex_windows_wgl
alias_example_complex_windows_wgl_native:
	mkdir -p make/output
	make/scripts/makefile_ln.sh makefile_example_complex_windows_wgl_native
alias_example_complex_x11_glx:
	mkdir -p make/output
	make/scripts/makefile_ln.sh makefile_example_complex_x11_glx



# library makefiles targets
## named & regular makefile targets for release library builds
makefile_release_lib_macos_software:
	make/lib/gen/gen_macos.sh release software osxcross osxcross

makefile_release_lib_macos_software_native:
	make/lib/gen/gen_macos.sh release software native native

makefile_release_lib_windows_software:
	make/lib/gen/gen_windows_mingw.sh release software

makefile_release_lib_windows_software_native:
	make/lib/gen/gen_windows_msvc.sh release software

makefile_release_lib_wayland_software:
	make/lib/gen/gen_wayland.sh release software

makefile_release_lib_x11_software:
	make/lib/gen/gen_x11.sh release software

makefile_release_lib_macos_egl:
	make/lib/gen/gen_macos.sh release egl osxcross osxcross

makefile_release_lib_macos_egl_native:
	make/lib/gen/gen_macos.sh release egl native native

makefile_release_lib_windows_egl:
	make/lib/gen/gen_windows_mingw.sh release egl

makefile_release_lib_windows_egl_native:
	make/lib/gen/gen_windows_msvc.sh release egl

makefile_release_lib_wayland_egl:
	make/lib/gen/gen_wayland.sh release egl

makefile_release_lib_x11_egl:
	make/lib/gen/gen_x11.sh release egl

makefile_release_lib_windows_wgl:
	make/lib/gen/gen_windows_mingw.sh release wgl

makefile_release_lib_windows_wgl_native:
	make/lib/gen/gen_windows_msvc.sh release wgl

makefile_release_lib_x11_glx:
	make/lib/gen/gen_x11.sh release glx


## named makefile targets for development library builds
makefile_development_lib_macos_software:
	make/lib/gen/gen_macos.sh development software osxcross osxcross

makefile_development_lib_macos_software_native:
	make/lib/gen/gen_macos.sh development software native native

makefile_development_lib_windows_software:
	make/lib/gen/gen_windows_mingw.sh development software

makefile_development_lib_windows_software_native:
	make/lib/gen/gen_windows_msvc.sh development software

makefile_development_lib_wayland_software:
	make/lib/gen/gen_wayland.sh development software

makefile_development_lib_x11_software:
	make/lib/gen/gen_x11.sh development software

makefile_development_lib_macos_egl:
	make/lib/gen/gen_macos.sh development egl osxcross osxcross

makefile_development_lib_macos_egl_native:
	make/lib/gen/gen_macos.sh development egl native native

makefile_development_lib_windows_egl:
	make/lib/gen/gen_windows_mingw.sh development egl

makefile_development_lib_windows_egl_native:
	make/lib/gen/gen_windows_msvc.sh development egl

makefile_development_lib_wayland_egl:
	make/lib/gen/gen_wayland.sh development egl

makefile_development_lib_x11_egl:
	make/lib/gen/gen_x11.sh development egl

makefile_development_lib_windows_wgl:
	make/lib/gen/gen_windows_mingw.sh development wgl

makefile_development_lib_windows_wgl_native:
	make/lib/gen/gen_windows_msvc.sh development wgl

makefile_development_lib_x11_glx:
	make/lib/gen/gen_x11.sh development glx


## named makefile targets for sanitized library builds
makefile_sanitized_memory_undefined_lib_macos_software:
	make/lib/gen/gen_macos.sh sanitized_memory_undefined software osxcross osxcross

makefile_sanitized_memory_undefined_lib_macos_software_native:
	make/lib/gen/gen_macos.sh sanitized_memory_undefined software native native

makefile_sanitized_memory_undefined_lib_wayland_software:
	make/lib/gen/gen_wayland.sh sanitized_memory_undefined software

makefile_sanitized_memory_undefined_lib_x11_software:
	make/lib/gen/gen_x11.sh sanitized_memory_undefined software

makefile_sanitized_memory_undefined_lib_macos_egl:
	make/lib/gen/gen_macos.sh sanitized_memory_undefined egl osxcross osxcross

makefile_sanitized_memory_undefined_lib_macos_egl_native:
	make/lib/gen/gen_macos.sh sanitized_memory_undefined egl native native

makefile_sanitized_memory_undefined_lib_wayland_egl:
	make/lib/gen/gen_wayland.sh sanitized_memory_undefined egl

makefile_sanitized_memory_undefined_lib_x11_egl:
	make/lib/gen/gen_x11.sh sanitized_memory_undefined egl

makefile_sanitized_memory_undefined_lib_x11_glx:
	make/lib/gen/gen_x11.sh sanitized_memory_undefined glx

makefile_sanitized_address_lib_macos_software:
	make/lib/gen/gen_macos.sh sanitized_address software osxcross osxcross

makefile_sanitized_address_lib_macos_software_native:
	make/lib/gen/gen_macos.sh sanitized_address software native native

makefile_sanitized_address_lib_wayland_software:
	make/lib/gen/gen_wayland.sh sanitized_address software

makefile_sanitized_address_lib_x11_software:
	make/lib/gen/gen_x11.sh sanitized_address software

makefile_sanitized_address_lib_macos_egl:
	make/lib/gen/gen_macos.sh sanitized_address egl osxcross osxcross

makefile_sanitized_address_lib_macos_egl_native:
	make/lib/gen/gen_macos.sh sanitized_address egl native native

makefile_sanitized_address_lib_wayland_egl:
	make/lib/gen/gen_wayland.sh sanitized_address egl

makefile_sanitized_address_lib_x11_egl:
	make/lib/gen/gen_x11.sh sanitized_address egl

makefile_sanitized_address_lib_x11_glx:
	make/lib/gen/gen_x11.sh sanitized_address glx

makefile_sanitized_thread_lib_macos_software:
	make/lib/gen/gen_macos.sh sanitized_thread software osxcross osxcross

makefile_sanitized_thread_lib_macos_software_native:
	make/lib/gen/gen_macos.sh sanitized_thread software native native

makefile_sanitized_thread_lib_wayland_software:
	make/lib/gen/gen_wayland.sh sanitized_thread software

makefile_sanitized_thread_lib_x11_software:
	make/lib/gen/gen_x11.sh sanitized_thread software

makefile_sanitized_thread_lib_macos_egl:
	make/lib/gen/gen_macos.sh sanitized_thread egl osxcross osxcross

makefile_sanitized_thread_lib_macos_egl_native:
	make/lib/gen/gen_macos.sh sanitized_thread egl native native

makefile_sanitized_thread_lib_wayland_egl:
	make/lib/gen/gen_wayland.sh sanitized_thread egl

makefile_sanitized_thread_lib_x11_egl:
	make/lib/gen/gen_x11.sh sanitized_thread egl

makefile_sanitized_thread_lib_x11_glx:
	make/lib/gen/gen_x11.sh sanitized_thread glx



# simple example makefiles targets (static)
## named & regular makefile targets for release simple example builds (static)
makefile_release_example_simple_macos_software_static:
	make/example/simple/gen/gen_macos.sh release software osxcross osxcross static

makefile_release_example_simple_macos_software_native_static:
	make/example/simple/gen/gen_macos.sh release software native native static

makefile_release_example_simple_windows_software_static:
	make/example/simple/gen/gen_windows_mingw.sh release software static

makefile_release_example_simple_windows_software_native_static:
	make/example/simple/gen/gen_windows_msvc.sh release software

makefile_release_example_simple_wayland_software_static:
	make/example/simple/gen/gen_wayland.sh release software static

makefile_release_example_simple_x11_software_static:
	make/example/simple/gen/gen_x11.sh release software static

makefile_release_example_simple_macos_egl_static:
	make/example/simple/gen/gen_macos.sh release egl osxcross osxcross static

makefile_release_example_simple_macos_egl_native_static:
	make/example/simple/gen/gen_macos.sh release egl native native static

makefile_release_example_simple_windows_egl_static:
	make/example/simple/gen/gen_windows_mingw.sh release egl static

makefile_release_example_simple_windows_egl_native_static:
	make/example/simple/gen/gen_windows_msvc.sh release egl

makefile_release_example_simple_wayland_egl_static:
	make/example/simple/gen/gen_wayland.sh release egl static

makefile_release_example_simple_x11_egl_static:
	make/example/simple/gen/gen_x11.sh release egl static

makefile_release_example_simple_windows_wgl_static:
	make/example/simple/gen/gen_windows_mingw.sh release wgl static

makefile_release_example_simple_windows_wgl_native_static:
	make/example/simple/gen/gen_windows_msvc.sh release wgl

makefile_release_example_simple_x11_glx_static:
	make/example/simple/gen/gen_x11.sh release glx static


## named makefile targets for development simple example builds (static)
makefile_development_example_simple_macos_software_static:
	make/example/simple/gen/gen_macos.sh development software osxcross osxcross static

makefile_development_example_simple_macos_software_native_static:
	make/example/simple/gen/gen_macos.sh development software native native static

makefile_development_example_simple_windows_software_static:
	make/example/simple/gen/gen_windows_mingw.sh development software static

makefile_development_example_simple_windows_software_native_static:
	make/example/simple/gen/gen_windows_msvc.sh development software

makefile_development_example_simple_wayland_software_static:
	make/example/simple/gen/gen_wayland.sh development software static

makefile_development_example_simple_x11_software_static:
	make/example/simple/gen/gen_x11.sh development software static

makefile_development_example_simple_macos_egl_static:
	make/example/simple/gen/gen_macos.sh development egl osxcross osxcross static

makefile_development_example_simple_macos_egl_native_static:
	make/example/simple/gen/gen_macos.sh development egl native native static

makefile_development_example_simple_windows_egl_static:
	make/example/simple/gen/gen_windows_mingw.sh development egl static

makefile_development_example_simple_windows_egl_native_static:
	make/example/simple/gen/gen_windows_msvc.sh development egl

makefile_development_example_simple_wayland_egl_static:
	make/example/simple/gen/gen_wayland.sh development egl static

makefile_development_example_simple_x11_egl_static:
	make/example/simple/gen/gen_x11.sh development egl static

makefile_development_example_simple_windows_wgl_static:
	make/example/simple/gen/gen_windows_mingw.sh development wgl static

makefile_development_example_simple_windows_wgl_native_static:
	make/example/simple/gen/gen_windows_msvc.sh development wgl

makefile_development_example_simple_x11_glx_static:
	make/example/simple/gen/gen_x11.sh development glx static


## named makefile targets for sanitized simple example builds (static)
makefile_sanitized_memory_undefined_example_simple_macos_software_static:
	make/example/simple/gen/gen_macos.sh sanitized_memory_undefined software osxcross osxcross static

makefile_sanitized_memory_undefined_example_simple_macos_software_native_static:
	make/example/simple/gen/gen_macos.sh sanitized_memory_undefined software native native static

makefile_sanitized_memory_undefined_example_simple_wayland_software_static:
	make/example/simple/gen/gen_wayland.sh sanitized_memory_undefined software static

makefile_sanitized_memory_undefined_example_simple_x11_software_static:
	make/example/simple/gen/gen_x11.sh sanitized_memory_undefined software static

makefile_sanitized_memory_undefined_example_simple_macos_egl_static:
	make/example/simple/gen/gen_macos.sh sanitized_memory_undefined egl osxcross osxcross static

makefile_sanitized_memory_undefined_example_simple_macos_egl_native_static:
	make/example/simple/gen/gen_macos.sh sanitized_memory_undefined egl native native static

makefile_sanitized_memory_undefined_example_simple_wayland_egl_static:
	make/example/simple/gen/gen_wayland.sh sanitized_memory_undefined egl static

makefile_sanitized_memory_undefined_example_simple_x11_egl_static:
	make/example/simple/gen/gen_x11.sh sanitized_memory_undefined egl static

makefile_sanitized_memory_undefined_example_simple_x11_glx_static:
	make/example/simple/gen/gen_x11.sh sanitized_memory_undefined glx static

makefile_sanitized_address_example_simple_macos_software_static:
	make/example/simple/gen/gen_macos.sh sanitized_address software osxcross osxcross static

makefile_sanitized_address_example_simple_macos_software_native_static:
	make/example/simple/gen/gen_macos.sh sanitized_address software native native static

makefile_sanitized_address_example_simple_wayland_software_static:
	make/example/simple/gen/gen_wayland.sh sanitized_address software static

makefile_sanitized_address_example_simple_x11_software_static:
	make/example/simple/gen/gen_x11.sh sanitized_address software static

makefile_sanitized_address_example_simple_macos_egl_static:
	make/example/simple/gen/gen_macos.sh sanitized_address egl osxcross osxcross static

makefile_sanitized_address_example_simple_macos_egl_native_static:
	make/example/simple/gen/gen_macos.sh sanitized_address egl native native static

makefile_sanitized_address_example_simple_wayland_egl_static:
	make/example/simple/gen/gen_wayland.sh sanitized_address egl static

makefile_sanitized_address_example_simple_x11_egl_static:
	make/example/simple/gen/gen_x11.sh sanitized_address egl static

makefile_sanitized_address_example_simple_x11_glx_static:
	make/example/simple/gen/gen_x11.sh sanitized_address glx static

makefile_sanitized_thread_example_simple_macos_software_static:
	make/example/simple/gen/gen_macos.sh sanitized_thread software osxcross osxcross static

makefile_sanitized_thread_example_simple_macos_software_native_static:
	make/example/simple/gen/gen_macos.sh sanitized_thread software native native static

makefile_sanitized_thread_example_simple_wayland_software_static:
	make/example/simple/gen/gen_wayland.sh sanitized_thread software static

makefile_sanitized_thread_example_simple_x11_software_static:
	make/example/simple/gen/gen_x11.sh sanitized_thread software static

makefile_sanitized_thread_example_simple_macos_egl_static:
	make/example/simple/gen/gen_macos.sh sanitized_thread egl osxcross osxcross static

makefile_sanitized_thread_example_simple_macos_egl_native_static:
	make/example/simple/gen/gen_macos.sh sanitized_thread egl native native static

makefile_sanitized_thread_example_simple_wayland_egl_static:
	make/example/simple/gen/gen_wayland.sh sanitized_thread egl static

makefile_sanitized_thread_example_simple_x11_egl_static:
	make/example/simple/gen/gen_x11.sh sanitized_thread egl static

makefile_sanitized_thread_example_simple_x11_glx_static:
	make/example/simple/gen/gen_x11.sh sanitized_thread glx static



# simple example makefiles targets (shared)
## named & regular makefile targets for release simple example builds (shared)
makefile_release_example_simple_macos_software_shared:
	make/example/simple/gen/gen_macos.sh release software osxcross osxcross shared

makefile_release_example_simple_macos_software_native_shared:
	make/example/simple/gen/gen_macos.sh release software native native shared

makefile_release_example_simple_windows_software_shared:
	make/example/simple/gen/gen_windows_mingw.sh release software shared

makefile_release_example_simple_windows_software_native_shared:
	make/example/simple/gen/gen_windows_msvc.sh release software

makefile_release_example_simple_wayland_software_shared:
	make/example/simple/gen/gen_wayland.sh release software shared

makefile_release_example_simple_x11_software_shared:
	make/example/simple/gen/gen_x11.sh release software shared

makefile_release_example_simple_macos_egl_shared:
	make/example/simple/gen/gen_macos.sh release egl osxcross osxcross shared

makefile_release_example_simple_macos_egl_native_shared:
	make/example/simple/gen/gen_macos.sh release egl native native shared

makefile_release_example_simple_windows_egl_shared:
	make/example/simple/gen/gen_windows_mingw.sh release egl shared

makefile_release_example_simple_windows_egl_native_shared:
	make/example/simple/gen/gen_windows_msvc.sh release egl

makefile_release_example_simple_wayland_egl_shared:
	make/example/simple/gen/gen_wayland.sh release egl shared

makefile_release_example_simple_x11_egl_shared:
	make/example/simple/gen/gen_x11.sh release egl shared

makefile_release_example_simple_windows_wgl_shared:
	make/example/simple/gen/gen_windows_mingw.sh release wgl shared

makefile_release_example_simple_windows_wgl_native_shared:
	make/example/simple/gen/gen_windows_msvc.sh release wgl

makefile_release_example_simple_x11_glx_shared:
	make/example/simple/gen/gen_x11.sh release glx shared


## named makefile targets for development simple example builds (shared)
makefile_development_example_simple_macos_software_shared:
	make/example/simple/gen/gen_macos.sh development software osxcross osxcross shared

makefile_development_example_simple_macos_software_native_shared:
	make/example/simple/gen/gen_macos.sh development software native native shared

makefile_development_example_simple_windows_software_shared:
	make/example/simple/gen/gen_windows_mingw.sh development software shared

makefile_development_example_simple_windows_software_native_shared:
	make/example/simple/gen/gen_windows_msvc.sh development software

makefile_development_example_simple_wayland_software_shared:
	make/example/simple/gen/gen_wayland.sh development software shared

makefile_development_example_simple_x11_software_shared:
	make/example/simple/gen/gen_x11.sh development software shared

makefile_development_example_simple_macos_egl_shared:
	make/example/simple/gen/gen_macos.sh development egl osxcross osxcross shared

makefile_development_example_simple_macos_egl_native_shared:
	make/example/simple/gen/gen_macos.sh development egl native native shared

makefile_development_example_simple_windows_egl_shared:
	make/example/simple/gen/gen_windows_mingw.sh development egl shared

makefile_development_example_simple_windows_egl_native_shared:
	make/example/simple/gen/gen_windows_msvc.sh development egl

makefile_development_example_simple_wayland_egl_shared:
	make/example/simple/gen/gen_wayland.sh development egl shared

makefile_development_example_simple_x11_egl_shared:
	make/example/simple/gen/gen_x11.sh development egl shared

makefile_development_example_simple_windows_wgl_shared:
	make/example/simple/gen/gen_windows_mingw.sh development wgl shared

makefile_development_example_simple_windows_wgl_native_shared:
	make/example/simple/gen/gen_windows_msvc.sh development wgl

makefile_development_example_simple_x11_glx_shared:
	make/example/simple/gen/gen_x11.sh development glx shared


## named makefile targets for sanitized simple example builds (shared)
makefile_sanitized_memory_undefined_example_simple_macos_software_shared:
	make/example/simple/gen/gen_macos.sh sanitized_memory_undefined software osxcross osxcross shared

makefile_sanitized_memory_undefined_example_simple_macos_software_native_shared:
	make/example/simple/gen/gen_macos.sh sanitized_memory_undefined software native native shared

makefile_sanitized_memory_undefined_example_simple_wayland_software_shared:
	make/example/simple/gen/gen_wayland.sh sanitized_memory_undefined software shared

makefile_sanitized_memory_undefined_example_simple_x11_software_shared:
	make/example/simple/gen/gen_x11.sh sanitized_memory_undefined software shared

makefile_sanitized_memory_undefined_example_simple_macos_egl_shared:
	make/example/simple/gen/gen_macos.sh sanitized_memory_undefined egl osxcross osxcross shared

makefile_sanitized_memory_undefined_example_simple_macos_egl_native_shared:
	make/example/simple/gen/gen_macos.sh sanitized_memory_undefined egl native native shared

makefile_sanitized_memory_undefined_example_simple_wayland_egl_shared:
	make/example/simple/gen/gen_wayland.sh sanitized_memory_undefined egl shared

makefile_sanitized_memory_undefined_example_simple_x11_egl_shared:
	make/example/simple/gen/gen_x11.sh sanitized_memory_undefined egl shared

makefile_sanitized_memory_undefined_example_simple_x11_glx_shared:
	make/example/simple/gen/gen_x11.sh sanitized_memory_undefined glx shared

makefile_sanitized_address_example_simple_macos_software_shared:
	make/example/simple/gen/gen_macos.sh sanitized_address software osxcross osxcross shared

makefile_sanitized_address_example_simple_macos_software_native_shared:
	make/example/simple/gen/gen_macos.sh sanitized_address software native native shared

makefile_sanitized_address_example_simple_wayland_software_shared:
	make/example/simple/gen/gen_wayland.sh sanitized_address software shared

makefile_sanitized_address_example_simple_x11_software_shared:
	make/example/simple/gen/gen_x11.sh sanitized_address software shared

makefile_sanitized_address_example_simple_macos_egl_shared:
	make/example/simple/gen/gen_macos.sh sanitized_address egl osxcross osxcross shared

makefile_sanitized_address_example_simple_macos_egl_native_shared:
	make/example/simple/gen/gen_macos.sh sanitized_address egl native native shared

makefile_sanitized_address_example_simple_wayland_egl_shared:
	make/example/simple/gen/gen_wayland.sh sanitized_address egl shared

makefile_sanitized_address_example_simple_x11_egl_shared:
	make/example/simple/gen/gen_x11.sh sanitized_address egl shared

makefile_sanitized_address_example_simple_x11_glx_shared:
	make/example/simple/gen/gen_x11.sh sanitized_address glx shared

makefile_sanitized_thread_example_simple_macos_software_shared:
	make/example/simple/gen/gen_macos.sh sanitized_thread software osxcross osxcross shared

makefile_sanitized_thread_example_simple_macos_software_native_shared:
	make/example/simple/gen/gen_macos.sh sanitized_thread software native native shared

makefile_sanitized_thread_example_simple_wayland_software_shared:
	make/example/simple/gen/gen_wayland.sh sanitized_thread software shared

makefile_sanitized_thread_example_simple_x11_software_shared:
	make/example/simple/gen/gen_x11.sh sanitized_thread software shared

makefile_sanitized_thread_example_simple_macos_egl_shared:
	make/example/simple/gen/gen_macos.sh sanitized_thread egl osxcross osxcross shared

makefile_sanitized_thread_example_simple_macos_egl_native_shared:
	make/example/simple/gen/gen_macos.sh sanitized_thread egl native native shared

makefile_sanitized_thread_example_simple_wayland_egl_shared:
	make/example/simple/gen/gen_wayland.sh sanitized_thread egl shared

makefile_sanitized_thread_example_simple_x11_egl_shared:
	make/example/simple/gen/gen_x11.sh sanitized_thread egl shared

makefile_sanitized_thread_example_simple_x11_glx_shared:
	make/example/simple/gen/gen_x11.sh sanitized_thread glx shared



# complex example makefiles targets (static)
## named & regular makefile targets for release complex example builds (static)
makefile_release_example_complex_macos_software_static:
	make/example/complex/gen/gen_macos.sh release software osxcross osxcross static

makefile_release_example_complex_macos_software_native_static:
	make/example/complex/gen/gen_macos.sh release software native native static

makefile_release_example_complex_windows_software_static:
	make/example/complex/gen/gen_windows_mingw.sh release software static

makefile_release_example_complex_windows_software_native_static:
	make/example/complex/gen/gen_windows_msvc.sh release software

makefile_release_example_complex_wayland_software_static:
	make/example/complex/gen/gen_wayland.sh release software static

makefile_release_example_complex_x11_software_static:
	make/example/complex/gen/gen_x11.sh release software static

makefile_release_example_complex_macos_egl_static:
	make/example/complex/gen/gen_macos.sh release egl osxcross osxcross static

makefile_release_example_complex_macos_egl_native_static:
	make/example/complex/gen/gen_macos.sh release egl native native static

makefile_release_example_complex_windows_egl_static:
	make/example/complex/gen/gen_windows_mingw.sh release egl static

makefile_release_example_complex_windows_egl_native_static:
	make/example/complex/gen/gen_windows_msvc.sh release egl

makefile_release_example_complex_wayland_egl_static:
	make/example/complex/gen/gen_wayland.sh release egl static

makefile_release_example_complex_x11_egl_static:
	make/example/complex/gen/gen_x11.sh release egl static

makefile_release_example_complex_windows_wgl_static:
	make/example/complex/gen/gen_windows_mingw.sh release wgl static

makefile_release_example_complex_windows_wgl_native_static:
	make/example/complex/gen/gen_windows_msvc.sh release wgl

makefile_release_example_complex_x11_glx_static:
	make/example/complex/gen/gen_x11.sh release glx static


## named makefile targets for development complex example builds (static)
makefile_development_example_complex_macos_software_static:
	make/example/complex/gen/gen_macos.sh development software osxcross osxcross static

makefile_development_example_complex_macos_software_native_static:
	make/example/complex/gen/gen_macos.sh development software native native static

makefile_development_example_complex_windows_software_static:
	make/example/complex/gen/gen_windows_mingw.sh development software static

makefile_development_example_complex_windows_software_native_static:
	make/example/complex/gen/gen_windows_msvc.sh development software

makefile_development_example_complex_wayland_software_static:
	make/example/complex/gen/gen_wayland.sh development software static

makefile_development_example_complex_x11_software_static:
	make/example/complex/gen/gen_x11.sh development software static

makefile_development_example_complex_macos_egl_static:
	make/example/complex/gen/gen_macos.sh development egl osxcross osxcross static

makefile_development_example_complex_macos_egl_native_static:
	make/example/complex/gen/gen_macos.sh development egl native native static

makefile_development_example_complex_windows_egl_static:
	make/example/complex/gen/gen_windows_mingw.sh development egl static

makefile_development_example_complex_windows_egl_native_static:
	make/example/complex/gen/gen_windows_msvc.sh development egl

makefile_development_example_complex_wayland_egl_static:
	make/example/complex/gen/gen_wayland.sh development egl static

makefile_development_example_complex_x11_egl_static:
	make/example/complex/gen/gen_x11.sh development egl static

makefile_development_example_complex_windows_wgl_static:
	make/example/complex/gen/gen_windows_mingw.sh development wgl static

makefile_development_example_complex_windows_wgl_native_static:
	make/example/complex/gen/gen_windows_msvc.sh development wgl

makefile_development_example_complex_x11_glx_static:
	make/example/complex/gen/gen_x11.sh development glx static


## named makefile targets for sanitized complex example builds (static)
makefile_sanitized_memory_undefined_example_complex_macos_software_static:
	make/example/complex/gen/gen_macos.sh sanitized_memory_undefined software osxcross osxcross static

makefile_sanitized_memory_undefined_example_complex_macos_software_native_static:
	make/example/complex/gen/gen_macos.sh sanitized_memory_undefined software native native static

makefile_sanitized_memory_undefined_example_complex_wayland_software_static:
	make/example/complex/gen/gen_wayland.sh sanitized_memory_undefined software static

makefile_sanitized_memory_undefined_example_complex_x11_software_static:
	make/example/complex/gen/gen_x11.sh sanitized_memory_undefined software static

makefile_sanitized_memory_undefined_example_complex_macos_egl_static:
	make/example/complex/gen/gen_macos.sh sanitized_memory_undefined egl osxcross osxcross static

makefile_sanitized_memory_undefined_example_complex_macos_egl_native_static:
	make/example/complex/gen/gen_macos.sh sanitized_memory_undefined egl native native static

makefile_sanitized_memory_undefined_example_complex_wayland_egl_static:
	make/example/complex/gen/gen_wayland.sh sanitized_memory_undefined egl static

makefile_sanitized_memory_undefined_example_complex_x11_egl_static:
	make/example/complex/gen/gen_x11.sh sanitized_memory_undefined egl static

makefile_sanitized_memory_undefined_example_complex_x11_glx_static:
	make/example/complex/gen/gen_x11.sh sanitized_memory_undefined glx static

makefile_sanitized_address_example_complex_macos_software_static:
	make/example/complex/gen/gen_macos.sh sanitized_address software osxcross osxcross static

makefile_sanitized_address_example_complex_macos_software_native_static:
	make/example/complex/gen/gen_macos.sh sanitized_address software native native static

makefile_sanitized_address_example_complex_wayland_software_static:
	make/example/complex/gen/gen_wayland.sh sanitized_address software static

makefile_sanitized_address_example_complex_x11_software_static:
	make/example/complex/gen/gen_x11.sh sanitized_address software static

makefile_sanitized_address_example_complex_macos_egl_static:
	make/example/complex/gen/gen_macos.sh sanitized_address egl osxcross osxcross static

makefile_sanitized_address_example_complex_macos_egl_native_static:
	make/example/complex/gen/gen_macos.sh sanitized_address egl native native static

makefile_sanitized_address_example_complex_wayland_egl_static:
	make/example/complex/gen/gen_wayland.sh sanitized_address egl static

makefile_sanitized_address_example_complex_x11_egl_static:
	make/example/complex/gen/gen_x11.sh sanitized_address egl static

makefile_sanitized_address_example_complex_x11_glx_static:
	make/example/complex/gen/gen_x11.sh sanitized_address glx static

makefile_sanitized_thread_example_complex_macos_software_static:
	make/example/complex/gen/gen_macos.sh sanitized_thread software osxcross osxcross static

makefile_sanitized_thread_example_complex_macos_software_native_static:
	make/example/complex/gen/gen_macos.sh sanitized_thread software native native static

makefile_sanitized_thread_example_complex_wayland_software_static:
	make/example/complex/gen/gen_wayland.sh sanitized_thread software static

makefile_sanitized_thread_example_complex_x11_software_static:
	make/example/complex/gen/gen_x11.sh sanitized_thread software static

makefile_sanitized_thread_example_complex_macos_egl_static:
	make/example/complex/gen/gen_macos.sh sanitized_thread egl osxcross osxcross static

makefile_sanitized_thread_example_complex_macos_egl_native_static:
	make/example/complex/gen/gen_macos.sh sanitized_thread egl native native static

makefile_sanitized_thread_example_complex_wayland_egl_static:
	make/example/complex/gen/gen_wayland.sh sanitized_thread egl static

makefile_sanitized_thread_example_complex_x11_egl_static:
	make/example/complex/gen/gen_x11.sh sanitized_thread egl static

makefile_sanitized_thread_example_complex_x11_glx_static:
	make/example/complex/gen/gen_x11.sh sanitized_thread glx static



# complex example makefiles targets (shared)
## named & regular makefile targets for release complex example builds (shared)
makefile_release_example_complex_macos_software_shared:
	make/example/complex/gen/gen_macos.sh release software osxcross osxcross shared

makefile_release_example_complex_macos_software_native_shared:
	make/example/complex/gen/gen_macos.sh release software native native shared

makefile_release_example_complex_windows_software_shared:
	make/example/complex/gen/gen_windows_mingw.sh release software shared

makefile_release_example_complex_windows_software_native_shared:
	make/example/complex/gen/gen_windows_msvc.sh release software

makefile_release_example_complex_wayland_software_shared:
	make/example/complex/gen/gen_wayland.sh release software shared

makefile_release_example_complex_x11_software_shared:
	make/example/complex/gen/gen_x11.sh release software shared

makefile_release_example_complex_macos_egl_shared:
	make/example/complex/gen/gen_macos.sh release egl osxcross osxcross shared

makefile_release_example_complex_macos_egl_native_shared:
	make/example/complex/gen/gen_macos.sh release egl native native shared

makefile_release_example_complex_windows_egl_shared:
	make/example/complex/gen/gen_windows_mingw.sh release egl shared

makefile_release_example_complex_windows_egl_native_shared:
	make/example/complex/gen/gen_windows_msvc.sh release egl

makefile_release_example_complex_wayland_egl_shared:
	make/example/complex/gen/gen_wayland.sh release egl shared

makefile_release_example_complex_x11_egl_shared:
	make/example/complex/gen/gen_x11.sh release egl shared

makefile_release_example_complex_windows_wgl_shared:
	make/example/complex/gen/gen_windows_mingw.sh release wgl shared

makefile_release_example_complex_windows_wgl_native_shared:
	make/example/complex/gen/gen_windows_msvc.sh release wgl

makefile_release_example_complex_x11_glx_shared:
	make/example/complex/gen/gen_x11.sh release glx shared


## named makefile targets for development complex example builds (shared)
makefile_development_example_complex_macos_software_shared:
	make/example/complex/gen/gen_macos.sh development software osxcross osxcross shared

makefile_development_example_complex_macos_software_native_shared:
	make/example/complex/gen/gen_macos.sh development software native native shared

makefile_development_example_complex_windows_software_shared:
	make/example/complex/gen/gen_windows_mingw.sh development software shared

makefile_development_example_complex_windows_software_native_shared:
	make/example/complex/gen/gen_windows_msvc.sh development software

makefile_development_example_complex_wayland_software_shared:
	make/example/complex/gen/gen_wayland.sh development software shared

makefile_development_example_complex_x11_software_shared:
	make/example/complex/gen/gen_x11.sh development software shared

makefile_development_example_complex_macos_egl_shared:
	make/example/complex/gen/gen_macos.sh development egl osxcross osxcross shared

makefile_development_example_complex_macos_egl_native_shared:
	make/example/complex/gen/gen_macos.sh development egl native native shared

makefile_development_example_complex_windows_egl_shared:
	make/example/complex/gen/gen_windows_mingw.sh development egl shared

makefile_development_example_complex_windows_egl_native_shared:
	make/example/complex/gen/gen_windows_msvc.sh development egl

makefile_development_example_complex_wayland_egl_shared:
	make/example/complex/gen/gen_wayland.sh development egl shared

makefile_development_example_complex_x11_egl_shared:
	make/example/complex/gen/gen_x11.sh development egl shared

makefile_development_example_complex_windows_wgl_shared:
	make/example/complex/gen/gen_windows_mingw.sh development wgl shared

makefile_development_example_complex_windows_wgl_native_shared:
	make/example/complex/gen/gen_windows_msvc.sh development wgl

makefile_development_example_complex_x11_glx_shared:
	make/example/complex/gen/gen_x11.sh development glx shared


## named makefile targets for sanitized complex example builds (shared)
makefile_sanitized_memory_undefined_example_complex_macos_software_shared:
	make/example/complex/gen/gen_macos.sh sanitized_memory_undefined software osxcross osxcross shared

makefile_sanitized_memory_undefined_example_complex_macos_software_native_shared:
	make/example/complex/gen/gen_macos.sh sanitized_memory_undefined software native native shared

makefile_sanitized_memory_undefined_example_complex_wayland_software_shared:
	make/example/complex/gen/gen_wayland.sh sanitized_memory_undefined software shared

makefile_sanitized_memory_undefined_example_complex_x11_software_shared:
	make/example/complex/gen/gen_x11.sh sanitized_memory_undefined software shared

makefile_sanitized_memory_undefined_example_complex_macos_egl_shared:
	make/example/complex/gen/gen_macos.sh sanitized_memory_undefined egl osxcross osxcross shared

makefile_sanitized_memory_undefined_example_complex_macos_egl_native_shared:
	make/example/complex/gen/gen_macos.sh sanitized_memory_undefined egl native native shared

makefile_sanitized_memory_undefined_example_complex_wayland_egl_shared:
	make/example/complex/gen/gen_wayland.sh sanitized_memory_undefined egl shared

makefile_sanitized_memory_undefined_example_complex_x11_egl_shared:
	make/example/complex/gen/gen_x11.sh sanitized_memory_undefined egl shared

makefile_sanitized_memory_undefined_example_complex_x11_glx_shared:
	make/example/complex/gen/gen_x11.sh sanitized_memory_undefined glx shared

makefile_sanitized_address_example_complex_macos_software_shared:
	make/example/complex/gen/gen_macos.sh sanitized_address software osxcross osxcross shared

makefile_sanitized_address_example_complex_macos_software_native_shared:
	make/example/complex/gen/gen_macos.sh sanitized_address software native native shared

makefile_sanitized_address_example_complex_wayland_software_shared:
	make/example/complex/gen/gen_wayland.sh sanitized_address software shared

makefile_sanitized_address_example_complex_x11_software_shared:
	make/example/complex/gen/gen_x11.sh sanitized_address software shared

makefile_sanitized_address_example_complex_macos_egl_shared:
	make/example/complex/gen/gen_macos.sh sanitized_address egl osxcross osxcross shared

makefile_sanitized_address_example_complex_macos_egl_native_shared:
	make/example/complex/gen/gen_macos.sh sanitized_address egl native native shared

makefile_sanitized_address_example_complex_wayland_egl_shared:
	make/example/complex/gen/gen_wayland.sh sanitized_address egl shared

makefile_sanitized_address_example_complex_x11_egl_shared:
	make/example/complex/gen/gen_x11.sh sanitized_address egl shared

makefile_sanitized_address_example_complex_x11_glx_shared:
	make/example/complex/gen/gen_x11.sh sanitized_address glx shared

makefile_sanitized_thread_example_complex_macos_software_shared:
	make/example/complex/gen/gen_macos.sh sanitized_thread software osxcross osxcross shared

makefile_sanitized_thread_example_complex_macos_software_native_shared:
	make/example/complex/gen/gen_macos.sh sanitized_thread software native native shared

makefile_sanitized_thread_example_complex_wayland_software_shared:
	make/example/complex/gen/gen_wayland.sh sanitized_thread software shared

makefile_sanitized_thread_example_complex_x11_software_shared:
	make/example/complex/gen/gen_x11.sh sanitized_thread software shared

makefile_sanitized_thread_example_complex_macos_egl_shared:
	make/example/complex/gen/gen_macos.sh sanitized_thread egl osxcross osxcross shared

makefile_sanitized_thread_example_complex_macos_egl_native_shared:
	make/example/complex/gen/gen_macos.sh sanitized_thread egl native native shared

makefile_sanitized_thread_example_complex_wayland_egl_shared:
	make/example/complex/gen/gen_wayland.sh sanitized_thread egl shared

makefile_sanitized_thread_example_complex_x11_egl_shared:
	make/example/complex/gen/gen_x11.sh sanitized_thread egl shared

makefile_sanitized_thread_example_complex_x11_glx_shared:
	make/example/complex/gen/gen_x11.sh sanitized_thread glx shared



# library binaries targets
## targets for software library binaries
build_lib_macos_software: \
make/output/makefile_lib_macos_software
	make/scripts/run_osxcross.sh make/output/makefile_lib_macos_software

build_lib_macos_software_native: \
make/output/makefile_lib_macos_software_native
	make/scripts/run_osxcross.sh make/output/makefile_lib_macos_software_native

build_lib_windows_software: \
make/output/makefile_lib_windows_software
	$(MAKE) -f make/output/makefile_lib_windows_software

build_lib_windows_software_native: \
make/output/makefile_lib_windows_software_native
	$(MAKE) -f make/output/makefile_lib_windows_software_native

build_lib_wayland_software: \
make/output/makefile_lib_wayland_software
	$(MAKE) -f make/output/makefile_lib_wayland_software

build_lib_x11_software: \
make/output/makefile_lib_x11_software
	$(MAKE) -f make/output/makefile_lib_x11_software


## targets for egl library binaries
build_lib_macos_egl: \
make/output/makefile_lib_macos_egl
	make/scripts/run_osxcross.sh make/output/makefile_lib_macos_egl

build_lib_macos_egl_native: \
make/output/makefile_lib_macos_egl_native
	make/scripts/run_osxcross.sh make/output/makefile_lib_macos_egl_native

build_lib_windows_egl: \
make/output/makefile_lib_windows_egl
	$(MAKE) -f make/output/makefile_lib_windows_egl

build_lib_windows_egl_native: \
make/output/makefile_lib_windows_egl_native
	$(MAKE) -f make/output/makefile_lib_windows_egl_native

build_lib_wayland_egl: \
make/output/makefile_lib_wayland_egl
	$(MAKE) -f make/output/makefile_lib_wayland_egl

build_lib_x11_egl: \
make/output/makefile_lib_x11_egl
	$(MAKE) -f make/output/makefile_lib_x11_egl

## targets for platform-specific gl-binder library binaries
build_lib_windows_wgl: \
make/output/makefile_lib_windows_wgl
	$(MAKE) -f make/output/makefile_lib_windows_wgl

build_lib_windows_wgl_native: \
make/output/makefile_lib_windows_wgl_native
	$(MAKE) -f make/output/makefile_lib_windows_wgl_native

build_lib_x11_glx: \
make/output/makefile_lib_x11_glx
	$(MAKE) -f make/output/makefile_lib_x11_glx



# simple example binaries targets
## targets for software simple example binaries
build_example_simple_macos_software_static: \
make/output/makefile_lib_macos_software \
make/output/makefile_example_simple_macos_software
	make/scripts/run_osxcross.sh make/output/makefile_lib_macos_software
	$(MAKE) -f make/output/makefile_lib_macos_software headers
	make/scripts/run_osxcross.sh make/output/makefile_example_simple_macos_software

build_example_simple_macos_software_native_static: \
make/output/makefile_lib_macos_software_native \
make/output/makefile_example_simple_macos_software_native
	make/scripts/run_osxcross.sh make/output/makefile_lib_macos_software_native
	$(MAKE) -f make/output/makefile_lib_macos_software_native headers
	make/scripts/run_osxcross.sh make/output/makefile_example_simple_macos_software_native

build_example_simple_windows_software_static: \
make/output/makefile_lib_windows_software \
make/output/makefile_example_simple_windows_software
	$(MAKE) -f make/output/makefile_lib_windows_software
	$(MAKE) -f make/output/makefile_lib_windows_software headers
	$(MAKE) -f make/output/makefile_example_simple_windows_software

build_example_simple_windows_software_native_static: \
make/output/makefile_lib_windows_software_native \
make/output/makefile_example_simple_windows_software_native
	$(MAKE) -f make/output/makefile_lib_windows_software_native
	$(MAKE) -f make/output/makefile_lib_windows_software_native headers
	$(MAKE) -f make/output/makefile_example_simple_windows_software_native

build_example_simple_wayland_software_static: \
make/output/makefile_lib_wayland_software \
make/output/makefile_example_simple_wayland_software
	$(MAKE) -f make/output/makefile_lib_wayland_software
	$(MAKE) -f make/output/makefile_lib_wayland_software headers
	$(MAKE) -f make/output/makefile_example_simple_wayland_software

build_example_simple_x11_software_static: \
make/output/makefile_lib_x11_software \
make/output/makefile_example_simple_x11_software
	$(MAKE) -f make/output/makefile_lib_x11_software
	$(MAKE) -f make/output/makefile_lib_x11_software headers
	$(MAKE) -f make/output/makefile_example_simple_x11_software


## targets for egl simple example binaries
build_example_simple_macos_egl_static: \
make/output/makefile_lib_macos_egl \
make/output/makefile_example_simple_macos_egl
	make/scripts/run_osxcross.sh make/output/makefile_lib_macos_egl
	$(MAKE) -f make/output/makefile_lib_macos_egl headers
	make/scripts/run_osxcross.sh make/output/makefile_example_simple_macos_egl

build_example_simple_macos_egl_native_static: \
make/output/makefile_lib_macos_egl_native \
make/output/makefile_example_simple_macos_egl_native
	make/scripts/run_osxcross.sh make/output/makefile_lib_macos_egl_native
	$(MAKE) -f make/output/makefile_lib_macos_egl_native headers
	make/scripts/run_osxcross.sh make/output/makefile_example_simple_macos_egl_native

build_example_simple_windows_egl_static: \
make/output/makefile_lib_windows_egl \
make/output/makefile_example_simple_windows_egl
	$(MAKE) -f make/output/makefile_lib_windows_egl
	$(MAKE) -f make/output/makefile_lib_windows_egl headers
	$(MAKE) -f make/output/makefile_example_simple_windows_egl

build_example_simple_windows_egl_native_static: \
make/output/makefile_lib_windows_egl_native \
make/output/makefile_example_simple_windows_egl_native
	$(MAKE) -f make/output/makefile_lib_windows_egl_native
	$(MAKE) -f make/output/makefile_lib_windows_egl_native headers
	$(MAKE) -f make/output/makefile_example_simple_windows_egl_native

build_example_simple_wayland_egl_static: \
make/output/makefile_lib_wayland_egl \
make/output/makefile_example_simple_wayland_egl
	$(MAKE) -f make/output/makefile_lib_wayland_egl
	$(MAKE) -f make/output/makefile_lib_wayland_egl headers
	$(MAKE) -f make/output/makefile_example_simple_wayland_egl

build_example_simple_x11_egl_static: \
make/output/makefile_lib_x11_egl \
make/output/makefile_example_simple_x11_egl
	$(MAKE) -f make/output/makefile_lib_x11_egl
	$(MAKE) -f make/output/makefile_lib_x11_egl headers
	$(MAKE) -f make/output/makefile_example_simple_x11_egl


## targets for platform-specific gl-binder simple example binaries
build_example_simple_windows_wgl_static: \
make/output/makefile_lib_windows_wgl \
make/output/makefile_example_simple_windows_wgl
	$(MAKE) -f make/output/makefile_lib_windows_wgl
	$(MAKE) -f make/output/makefile_lib_windows_wgl headers
	$(MAKE) -f make/output/makefile_example_simple_windows_wgl

build_example_simple_windows_wgl_native_static: \
make/output/makefile_lib_windows_wgl_native \
make/output/makefile_example_simple_windows_wgl_native
	$(MAKE) -f make/output/makefile_lib_windows_wgl_native
	$(MAKE) -f make/output/makefile_lib_windows_wgl_native headers
	$(MAKE) -f make/output/makefile_example_simple_windows_wgl_native

build_example_simple_x11_glx_static: \
make/output/makefile_lib_x11_glx \
make/output/makefile_example_simple_x11_glx
	$(MAKE) -f make/output/makefile_lib_x11_glx
	$(MAKE) -f make/output/makefile_lib_x11_glx headers
	$(MAKE) -f make/output/makefile_example_simple_x11_glx



# complex example binaries targets
## targets for software complex example binaries
build_example_complex_macos_software_static: \
make/output/makefile_lib_macos_software \
make/output/makefile_example_complex_macos_software
	make/scripts/run_osxcross.sh make/output/makefile_lib_macos_software
	$(MAKE) -f make/output/makefile_lib_macos_software headers
	make/scripts/run_osxcross.sh make/output/makefile_example_complex_macos_software

build_example_complex_macos_software_native_static: \
make/output/makefile_lib_macos_software_native \
make/output/makefile_example_complex_macos_software_native
	make/scripts/run_osxcross.sh make/output/makefile_lib_macos_software_native
	$(MAKE) -f make/output/makefile_lib_macos_software_native headers
	make/scripts/run_osxcross.sh make/output/makefile_example_complex_macos_software_native

build_example_complex_windows_software_static: \
make/output/makefile_lib_windows_software \
make/output/makefile_example_complex_windows_software
	$(MAKE) -f make/output/makefile_lib_windows_software
	$(MAKE) -f make/output/makefile_lib_windows_software headers
	$(MAKE) -f make/output/makefile_example_complex_windows_software

build_example_complex_windows_software_native_static: \
make/output/makefile_lib_windows_software_native \
make/output/makefile_example_complex_windows_software_native
	$(MAKE) -f make/output/makefile_lib_windows_software_native
	$(MAKE) -f make/output/makefile_lib_windows_software_native headers
	$(MAKE) -f make/output/makefile_example_complex_windows_software_native

build_example_complex_wayland_software_static: \
make/output/makefile_lib_wayland_software \
make/output/makefile_example_complex_wayland_software
	$(MAKE) -f make/output/makefile_lib_wayland_software
	$(MAKE) -f make/output/makefile_lib_wayland_software headers
	$(MAKE) -f make/output/makefile_example_complex_wayland_software

build_example_complex_x11_software_static: \
make/output/makefile_lib_x11_software \
make/output/makefile_example_complex_x11_software
	$(MAKE) -f make/output/makefile_lib_x11_software
	$(MAKE) -f make/output/makefile_lib_x11_software headers
	$(MAKE) -f make/output/makefile_example_complex_x11_software


## targets for egl complex example binaries
build_example_complex_macos_egl_static: \
make/output/makefile_lib_macos_egl \
make/output/makefile_example_complex_macos_egl
	make/scripts/run_osxcross.sh make/output/makefile_lib_macos_egl
	$(MAKE) -f make/output/makefile_lib_macos_egl headers
	make/scripts/run_osxcross.sh make/output/makefile_example_complex_macos_egl

build_example_complex_macos_egl_native_static: \
make/output/makefile_lib_macos_egl_native \
make/output/makefile_example_complex_macos_egl_native
	make/scripts/run_osxcross.sh make/output/makefile_lib_macos_egl_native
	$(MAKE) -f make/output/makefile_lib_macos_egl_native headers
	make/scripts/run_osxcross.sh make/output/makefile_example_complex_macos_egl_native

build_example_complex_windows_egl_static: \
make/output/makefile_lib_windows_egl \
make/output/makefile_example_complex_windows_egl
	$(MAKE) -f make/output/makefile_lib_windows_egl
	$(MAKE) -f make/output/makefile_lib_windows_egl headers
	$(MAKE) -f make/output/makefile_example_complex_windows_egl

build_example_complex_windows_egl_native_static: \
make/output/makefile_lib_windows_egl_native \
make/output/makefile_example_complex_windows_egl_native
	$(MAKE) -f make/output/makefile_lib_windows_egl_native
	$(MAKE) -f make/output/makefile_lib_windows_egl_native headers
	$(MAKE) -f make/output/makefile_example_complex_windows_egl_native

build_example_complex_wayland_egl_static: \
make/output/makefile_lib_wayland_egl \
make/output/makefile_example_complex_wayland_egl
	$(MAKE) -f make/output/makefile_lib_wayland_egl
	$(MAKE) -f make/output/makefile_lib_wayland_egl headers
	$(MAKE) -f make/output/makefile_example_complex_wayland_egl

build_example_complex_x11_egl_static: \
make/output/makefile_lib_x11_egl \
make/output/makefile_example_complex_x11_egl
	$(MAKE) -f make/output/makefile_lib_x11_egl
	$(MAKE) -f make/output/makefile_lib_x11_egl headers
	$(MAKE) -f make/output/makefile_example_complex_x11_egl


## targets for platform-specific gl-binder complex example binaries
build_example_complex_windows_wgl_static: \
make/output/makefile_lib_windows_wgl \
make/output/makefile_example_complex_windows_wgl
	$(MAKE) -f make/output/makefile_lib_windows_wgl
	$(MAKE) -f make/output/makefile_lib_windows_wgl headers
	$(MAKE) -f make/output/makefile_example_complex_windows_wgl

build_example_complex_windows_wgl_native_static: \
make/output/makefile_lib_windows_wgl_native \
make/output/makefile_example_complex_windows_wgl_native
	$(MAKE) -f make/output/makefile_lib_windows_wgl_native
	$(MAKE) -f make/output/makefile_lib_windows_wgl_native headers
	$(MAKE) -f make/output/makefile_example_complex_windows_wgl_native

build_example_complex_x11_glx_static: \
make/output/makefile_lib_x11_glx \
make/output/makefile_example_complex_x11_glx
	$(MAKE) -f make/output/makefile_lib_x11_glx
	$(MAKE) -f make/output/makefile_lib_x11_glx headers
	$(MAKE) -f make/output/makefile_example_complex_x11_glx



# simple example run targets
## targets for software simple example run
run_example_simple_macos_software_static: \
make/output/makefile_lib_macos_software \
make/output/makefile_example_simple_macos_software
	make/scripts/run_osxcross.sh make/output/makefile_lib_macos_software
	$(MAKE) -f make/output/makefile_lib_macos_software headers
	make/scripts/run_osxcross.sh make/output/makefile_example_simple_macos_software run

run_example_simple_macos_software_native_static: \
make/output/makefile_lib_macos_software_native \
make/output/makefile_example_simple_macos_software_native
	make/scripts/run_osxcross.sh make/output/makefile_lib_macos_software_native
	$(MAKE) -f make/output/makefile_lib_macos_software_native headers
	make/scripts/run_osxcross.sh make/output/makefile_example_simple_macos_software_native run

run_example_simple_windows_software_static: \
make/output/makefile_lib_windows_software \
make/output/makefile_example_simple_windows_software
	$(MAKE) -f make/output/makefile_lib_windows_software
	$(MAKE) -f make/output/makefile_lib_windows_software headers
	$(MAKE) -f make/output/makefile_example_simple_windows_software run

run_example_simple_windows_software_native_static: \
make/output/makefile_lib_windows_software_native \
make/output/makefile_example_simple_windows_software_native
	$(MAKE) -f make/output/makefile_lib_windows_software_native
	$(MAKE) -f make/output/makefile_lib_windows_software_native headers
	$(MAKE) -f make/output/makefile_example_simple_windows_software_native run

run_example_simple_wayland_software_static: \
make/output/makefile_lib_wayland_software \
make/output/makefile_example_simple_wayland_software
	$(MAKE) -f make/output/makefile_lib_wayland_software
	$(MAKE) -f make/output/makefile_lib_wayland_software headers
	$(MAKE) -f make/output/makefile_example_simple_wayland_software run

run_example_simple_x11_software_static: \
make/output/makefile_lib_x11_software \
make/output/makefile_example_simple_x11_software
	$(MAKE) -f make/output/makefile_lib_x11_software
	$(MAKE) -f make/output/makefile_lib_x11_software headers
	$(MAKE) -f make/output/makefile_example_simple_x11_software run


## targets for egl simple example run
run_example_simple_macos_egl_static: \
make/output/makefile_lib_macos_egl \
make/output/makefile_example_simple_macos_egl
	make/scripts/run_osxcross.sh make/output/makefile_lib_macos_egl
	$(MAKE) -f make/output/makefile_lib_macos_egl headers
	make/scripts/run_osxcross.sh make/output/makefile_example_simple_macos_egl run

run_example_simple_macos_egl_native_static: \
make/output/makefile_lib_macos_egl_native \
make/output/makefile_example_simple_macos_egl_native
	make/scripts/run_osxcross.sh make/output/makefile_lib_macos_egl_native
	$(MAKE) -f make/output/makefile_lib_macos_egl_native headers
	make/scripts/run_osxcross.sh make/output/makefile_example_simple_macos_egl_native run

run_example_simple_windows_egl_static: \
make/output/makefile_lib_windows_egl \
make/output/makefile_example_simple_windows_egl
	$(MAKE) -f make/output/makefile_lib_windows_egl
	$(MAKE) -f make/output/makefile_lib_windows_egl headers
	$(MAKE) -f make/output/makefile_example_simple_windows_egl run

run_example_simple_windows_egl_native_static: \
make/output/makefile_lib_windows_egl_native \
make/output/makefile_example_simple_windows_egl_native
	$(MAKE) -f make/output/makefile_lib_windows_egl_native
	$(MAKE) -f make/output/makefile_lib_windows_egl_native headers
	$(MAKE) -f make/output/makefile_example_simple_windows_egl_native run

run_example_simple_wayland_egl_static: \
make/output/makefile_lib_wayland_egl \
make/output/makefile_example_simple_wayland_egl
	$(MAKE) -f make/output/makefile_lib_wayland_egl
	$(MAKE) -f make/output/makefile_lib_wayland_egl headers
	$(MAKE) -f make/output/makefile_example_simple_wayland_egl run

run_example_simple_x11_egl_static: \
make/output/makefile_lib_x11_egl \
make/output/makefile_example_simple_x11_egl
	$(MAKE) -f make/output/makefile_lib_x11_egl
	$(MAKE) -f make/output/makefile_lib_x11_egl headers
	$(MAKE) -f make/output/makefile_example_simple_x11_egl run


## targets for platform-specific gl-binder simple example run
run_example_simple_windows_wgl_static: \
make/output/makefile_lib_windows_wgl \
make/output/makefile_example_simple_windows_wgl
	$(MAKE) -f make/output/makefile_lib_windows_wgl
	$(MAKE) -f make/output/makefile_lib_windows_wgl headers
	$(MAKE) -f make/output/makefile_example_simple_windows_wgl run

run_example_simple_windows_wgl_native_static: \
make/output/makefile_lib_windows_wgl_native \
make/output/makefile_example_simple_windows_wgl_native
	$(MAKE) -f make/output/makefile_lib_windows_wgl_native
	$(MAKE) -f make/output/makefile_lib_windows_wgl_native headers
	$(MAKE) -f make/output/makefile_example_simple_windows_wgl_native run

run_example_simple_x11_glx_static: \
make/output/makefile_lib_x11_glx \
make/output/makefile_example_simple_x11_glx
	$(MAKE) -f make/output/makefile_lib_x11_glx
	$(MAKE) -f make/output/makefile_lib_x11_glx headers
	$(MAKE) -f make/output/makefile_example_simple_x11_glx run



# complex example run targets
## targets for software complex example run
run_example_complex_macos_software_static: \
make/output/makefile_lib_macos_software \
make/output/makefile_example_complex_macos_software
	make/scripts/run_osxcross.sh make/output/makefile_lib_macos_software
	$(MAKE) -f make/output/makefile_lib_macos_software headers
	make/scripts/run_osxcross.sh make/output/makefile_example_complex_macos_software run

run_example_complex_macos_software_native_static: \
make/output/makefile_lib_macos_software_native \
make/output/makefile_example_complex_macos_software_native
	make/scripts/run_osxcross.sh make/output/makefile_lib_macos_software_native
	$(MAKE) -f make/output/makefile_lib_macos_software_native headers
	make/scripts/run_osxcross.sh make/output/makefile_example_complex_macos_software_native run

run_example_complex_windows_software_static: \
make/output/makefile_lib_windows_software \
make/output/makefile_example_complex_windows_software
	$(MAKE) -f make/output/makefile_lib_windows_software
	$(MAKE) -f make/output/makefile_lib_windows_software headers
	$(MAKE) -f make/output/makefile_example_complex_windows_software run

run_example_complex_windows_software_native_static: \
make/output/makefile_lib_windows_software_native \
make/output/makefile_example_complex_windows_software_native
	$(MAKE) -f make/output/makefile_lib_windows_software_native
	$(MAKE) -f make/output/makefile_lib_windows_software_native headers
	$(MAKE) -f make/output/makefile_example_complex_windows_software_native run

run_example_complex_wayland_software_static: \
make/output/makefile_lib_wayland_software \
make/output/makefile_example_complex_wayland_software
	$(MAKE) -f make/output/makefile_lib_wayland_software
	$(MAKE) -f make/output/makefile_lib_wayland_software headers
	$(MAKE) -f make/output/makefile_example_complex_wayland_software run

run_example_complex_x11_software_static: \
make/output/makefile_lib_x11_software \
make/output/makefile_example_complex_x11_software
	$(MAKE) -f make/output/makefile_lib_x11_software
	$(MAKE) -f make/output/makefile_lib_x11_software headers
	$(MAKE) -f make/output/makefile_example_complex_x11_software run


## targets for egl complex example run
run_example_complex_macos_egl_static: \
make/output/makefile_lib_macos_egl \
make/output/makefile_example_complex_macos_egl
	make/scripts/run_osxcross.sh make/output/makefile_lib_macos_egl
	$(MAKE) -f make/output/makefile_lib_macos_egl headers
	make/scripts/run_osxcross.sh make/output/makefile_example_complex_macos_egl run

run_example_complex_macos_egl_native_static: \
make/output/makefile_lib_macos_egl_native \
make/output/makefile_example_complex_macos_egl_native
	make/scripts/run_osxcross.sh make/output/makefile_lib_macos_egl_native
	$(MAKE) -f make/output/makefile_lib_macos_egl_native headers
	make/scripts/run_osxcross.sh make/output/makefile_example_complex_macos_egl_native run

run_example_complex_windows_egl_static: \
make/output/makefile_lib_windows_egl \
make/output/makefile_example_complex_windows_egl
	$(MAKE) -f make/output/makefile_lib_windows_egl
	$(MAKE) -f make/output/makefile_lib_windows_egl headers
	$(MAKE) -f make/output/makefile_example_complex_windows_egl run

run_example_complex_windows_egl_native_static: \
make/output/makefile_lib_windows_egl_native \
make/output/makefile_example_complex_windows_egl_native
	$(MAKE) -f make/output/makefile_lib_windows_egl_native
	$(MAKE) -f make/output/makefile_lib_windows_egl_native headers
	$(MAKE) -f make/output/makefile_example_complex_windows_egl_native run

run_example_complex_wayland_egl_static: \
make/output/makefile_lib_wayland_egl \
make/output/makefile_example_complex_wayland_egl
	$(MAKE) -f make/output/makefile_lib_wayland_egl
	$(MAKE) -f make/output/makefile_lib_wayland_egl headers
	$(MAKE) -f make/output/makefile_example_complex_wayland_egl run

run_example_complex_x11_egl_static: \
make/output/makefile_lib_x11_egl \
make/output/makefile_example_complex_x11_egl
	$(MAKE) -f make/output/makefile_lib_x11_egl
	$(MAKE) -f make/output/makefile_lib_x11_egl headers
	$(MAKE) -f make/output/makefile_example_complex_x11_egl run


## targets for platform-specific gl-binder complex example run
run_example_complex_windows_wgl_static: \
make/output/makefile_lib_windows_wgl \
make/output/makefile_example_complex_windows_wgl
	$(MAKE) -f make/output/makefile_lib_windows_wgl
	$(MAKE) -f make/output/makefile_lib_windows_wgl headers
	$(MAKE) -f make/output/makefile_example_complex_windows_wgl run

run_example_complex_windows_wgl_native_static: \
make/output/makefile_lib_windows_wgl_native \
make/output/makefile_example_complex_windows_wgl_native
	$(MAKE) -f make/output/makefile_lib_windows_wgl_native
	$(MAKE) -f make/output/makefile_lib_windows_wgl_native headers
	$(MAKE) -f make/output/makefile_example_complex_windows_wgl_native run

run_example_complex_x11_glx_static: \
make/output/makefile_lib_x11_glx \
make/output/makefile_example_complex_x11_glx
	$(MAKE) -f make/output/makefile_lib_x11_glx
	$(MAKE) -f make/output/makefile_lib_x11_glx headers
	$(MAKE) -f make/output/makefile_example_complex_x11_glx run



# simple example leak targets
## targets for software simple example leak
leak_example_simple_macos_software_static: \
make/output/makefile_lib_macos_software \
make/output/makefile_example_simple_macos_software
	make/scripts/run_osxcross.sh make/output/makefile_lib_macos_software
	$(MAKE) -f make/output/makefile_lib_macos_software headers
	make/scripts/run_osxcross.sh make/output/makefile_example_simple_macos_software leak

leak_example_simple_macos_software_native_static: \
make/output/makefile_lib_macos_software_native \
make/output/makefile_example_simple_macos_software_native
	make/scripts/run_osxcross.sh make/output/makefile_lib_macos_software_native
	$(MAKE) -f make/output/makefile_lib_macos_software_native headers
	make/scripts/run_osxcross.sh make/output/makefile_example_simple_macos_software_native leak

leak_example_simple_windows_software_static: \
make/output/makefile_lib_windows_software \
make/output/makefile_example_simple_windows_software
	$(MAKE) -f make/output/makefile_lib_windows_software
	$(MAKE) -f make/output/makefile_lib_windows_software headers
	$(MAKE) -f make/output/makefile_example_simple_windows_software leak

leak_example_simple_windows_software_native_static: \
make/output/makefile_lib_windows_software_native \
make/output/makefile_example_simple_windows_software_native
	$(MAKE) -f make/output/makefile_lib_windows_software_native
	$(MAKE) -f make/output/makefile_lib_windows_software_native headers
	$(MAKE) -f make/output/makefile_example_simple_windows_software_native leak

leak_example_simple_wayland_software_static: \
make/output/makefile_lib_wayland_software \
make/output/makefile_example_simple_wayland_software
	$(MAKE) -f make/output/makefile_lib_wayland_software
	$(MAKE) -f make/output/makefile_lib_wayland_software headers
	$(MAKE) -f make/output/makefile_example_simple_wayland_software leak

leak_example_simple_x11_software_static: \
make/output/makefile_lib_x11_software \
make/output/makefile_example_simple_x11_software
	$(MAKE) -f make/output/makefile_lib_x11_software
	$(MAKE) -f make/output/makefile_lib_x11_software headers
	$(MAKE) -f make/output/makefile_example_simple_x11_software leak


## targets for egl simple example leak
leak_example_simple_macos_egl_static: \
make/output/makefile_lib_macos_egl \
make/output/makefile_example_simple_macos_egl
	make/scripts/run_osxcross.sh make/output/makefile_lib_macos_egl
	$(MAKE) -f make/output/makefile_lib_macos_egl headers
	make/scripts/run_osxcross.sh make/output/makefile_example_simple_macos_egl leak

leak_example_simple_macos_egl_native_static: \
make/output/makefile_lib_macos_egl_native \
make/output/makefile_example_simple_macos_egl_native
	make/scripts/run_osxcross.sh make/output/makefile_lib_macos_egl_native
	$(MAKE) -f make/output/makefile_lib_macos_egl_native headers
	make/scripts/run_osxcross.sh make/output/makefile_example_simple_macos_egl_native leak

leak_example_simple_windows_egl_static: \
make/output/makefile_lib_windows_egl \
make/output/makefile_example_simple_windows_egl
	$(MAKE) -f make/output/makefile_lib_windows_egl
	$(MAKE) -f make/output/makefile_lib_windows_egl headers
	$(MAKE) -f make/output/makefile_example_simple_windows_egl leak

leak_example_simple_windows_egl_native_static: \
make/output/makefile_lib_windows_egl_native \
make/output/makefile_example_simple_windows_egl_native
	$(MAKE) -f make/output/makefile_lib_windows_egl_native
	$(MAKE) -f make/output/makefile_lib_windows_egl_native headers
	$(MAKE) -f make/output/makefile_example_simple_windows_egl_native leak

leak_example_simple_wayland_egl_static: \
make/output/makefile_lib_wayland_egl \
make/output/makefile_example_simple_wayland_egl
	$(MAKE) -f make/output/makefile_lib_wayland_egl
	$(MAKE) -f make/output/makefile_lib_wayland_egl headers
	$(MAKE) -f make/output/makefile_example_simple_wayland_egl leak

leak_example_simple_x11_egl_static: \
make/output/makefile_lib_x11_egl \
make/output/makefile_example_simple_x11_egl
	$(MAKE) -f make/output/makefile_lib_x11_egl
	$(MAKE) -f make/output/makefile_lib_x11_egl headers
	$(MAKE) -f make/output/makefile_example_simple_x11_egl leak


## targets for platform-specific gl-binder simple example leak
leak_example_simple_windows_wgl_static: \
make/output/makefile_lib_windows_wgl \
make/output/makefile_example_simple_windows_wgl
	$(MAKE) -f make/output/makefile_lib_windows_wgl
	$(MAKE) -f make/output/makefile_lib_windows_wgl headers
	$(MAKE) -f make/output/makefile_example_simple_windows_wgl leak

leak_example_simple_windows_wgl_native_static: \
make/output/makefile_lib_windows_wgl_native \
make/output/makefile_example_simple_windows_wgl_native
	$(MAKE) -f make/output/makefile_lib_windows_wgl_native
	$(MAKE) -f make/output/makefile_lib_windows_wgl_native headers
	$(MAKE) -f make/output/makefile_example_simple_windows_wgl_native leak

leak_example_simple_x11_glx_static: \
make/output/makefile_lib_x11_glx \
make/output/makefile_example_simple_x11_glx
	$(MAKE) -f make/output/makefile_lib_x11_glx
	$(MAKE) -f make/output/makefile_lib_x11_glx headers
	$(MAKE) -f make/output/makefile_example_simple_x11_glx leak



# complex example leak targets
## targets for software complex example leak
leak_example_complex_macos_software_static: \
make/output/makefile_lib_macos_software \
make/output/makefile_example_complex_macos_software
	make/scripts/run_osxcross.sh make/output/makefile_lib_macos_software
	$(MAKE) -f make/output/makefile_lib_macos_software headers
	make/scripts/run_osxcross.sh make/output/makefile_example_complex_macos_software leak

leak_example_complex_macos_software_native_static: \
make/output/makefile_lib_macos_software_native \
make/output/makefile_example_complex_macos_software_native
	make/scripts/run_osxcross.sh make/output/makefile_lib_macos_software_native
	$(MAKE) -f make/output/makefile_lib_macos_software_native headers
	make/scripts/run_osxcross.sh make/output/makefile_example_complex_macos_software_native leak

leak_example_complex_windows_software_static: \
make/output/makefile_lib_windows_software \
make/output/makefile_example_complex_windows_software
	$(MAKE) -f make/output/makefile_lib_windows_software
	$(MAKE) -f make/output/makefile_lib_windows_software headers
	$(MAKE) -f make/output/makefile_example_complex_windows_software leak

leak_example_complex_windows_software_native_static: \
make/output/makefile_lib_windows_software_native \
make/output/makefile_example_complex_windows_software_native
	$(MAKE) -f make/output/makefile_lib_windows_software_native
	$(MAKE) -f make/output/makefile_lib_windows_software_native headers
	$(MAKE) -f make/output/makefile_example_complex_windows_software_native leak

leak_example_complex_wayland_software_static: \
make/output/makefile_lib_wayland_software \
make/output/makefile_example_complex_wayland_software
	$(MAKE) -f make/output/makefile_lib_wayland_software
	$(MAKE) -f make/output/makefile_lib_wayland_software headers
	$(MAKE) -f make/output/makefile_example_complex_wayland_software leak

leak_example_complex_x11_software_static: \
make/output/makefile_lib_x11_software \
make/output/makefile_example_complex_x11_software
	$(MAKE) -f make/output/makefile_lib_x11_software
	$(MAKE) -f make/output/makefile_lib_x11_software headers
	$(MAKE) -f make/output/makefile_example_complex_x11_software leak


## targets for egl complex example leak
leak_example_complex_macos_egl_static: \
make/output/makefile_lib_macos_egl \
make/output/makefile_example_complex_macos_egl
	make/scripts/run_osxcross.sh make/output/makefile_lib_macos_egl
	$(MAKE) -f make/output/makefile_lib_macos_egl headers
	make/scripts/run_osxcross.sh make/output/makefile_example_complex_macos_egl leak

leak_example_complex_macos_egl_native_static: \
make/output/makefile_lib_macos_egl_native \
make/output/makefile_example_complex_macos_egl_native
	make/scripts/run_osxcross.sh make/output/makefile_lib_macos_egl_native
	$(MAKE) -f make/output/makefile_lib_macos_egl_native headers
	make/scripts/run_osxcross.sh make/output/makefile_example_complex_macos_egl_native leak

leak_example_complex_windows_egl_static: \
make/output/makefile_lib_windows_egl \
make/output/makefile_example_complex_windows_egl
	$(MAKE) -f make/output/makefile_lib_windows_egl
	$(MAKE) -f make/output/makefile_lib_windows_egl headers
	$(MAKE) -f make/output/makefile_example_complex_windows_egl leak

leak_example_complex_windows_egl_native_static: \
make/output/makefile_lib_windows_egl_native \
make/output/makefile_example_complex_windows_egl_native
	$(MAKE) -f make/output/makefile_lib_windows_egl_native
	$(MAKE) -f make/output/makefile_lib_windows_egl_native headers
	$(MAKE) -f make/output/makefile_example_complex_windows_egl_native leak

leak_example_complex_wayland_egl_static: \
make/output/makefile_lib_wayland_egl \
make/output/makefile_example_complex_wayland_egl
	$(MAKE) -f make/output/makefile_lib_wayland_egl
	$(MAKE) -f make/output/makefile_lib_wayland_egl headers
	$(MAKE) -f make/output/makefile_example_complex_wayland_egl leak

leak_example_complex_x11_egl_static: \
make/output/makefile_lib_x11_egl \
make/output/makefile_example_complex_x11_egl
	$(MAKE) -f make/output/makefile_lib_x11_egl
	$(MAKE) -f make/output/makefile_lib_x11_egl headers
	$(MAKE) -f make/output/makefile_example_complex_x11_egl leak


## targets for platform-specific gl-binder complex example leak
leak_example_complex_windows_wgl_static: \
make/output/makefile_lib_windows_wgl \
make/output/makefile_example_complex_windows_wgl
	$(MAKE) -f make/output/makefile_lib_windows_wgl
	$(MAKE) -f make/output/makefile_lib_windows_wgl headers
	$(MAKE) -f make/output/makefile_example_complex_windows_wgl leak

leak_example_complex_windows_wgl_native_static: \
make/output/makefile_lib_windows_wgl_native \
make/output/makefile_example_complex_windows_wgl_native
	$(MAKE) -f make/output/makefile_lib_windows_wgl_native
	$(MAKE) -f make/output/makefile_lib_windows_wgl_native headers
	$(MAKE) -f make/output/makefile_example_complex_windows_wgl_native leak

leak_example_complex_x11_glx_static: \
make/output/makefile_lib_x11_glx \
make/output/makefile_example_complex_x11_glx
	$(MAKE) -f make/output/makefile_lib_x11_glx
	$(MAKE) -f make/output/makefile_lib_x11_glx headers
	$(MAKE) -f make/output/makefile_example_complex_x11_glx leak
