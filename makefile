# utils
clean:
	make/scripts/clean.sh

clean_makefiles:
	make/scripts/clean_makefiles.sh

egl_get:
	make/scripts/egl_get.sh

pixmap_bin:
	make/scripts/pixmap_bin.sh

release_headers:
	make/scripts/release_headers.sh

release_zip:
	make/scripts/release_zip.sh

# library makefiles targets
makefile_lib_x11_software: \
make/output/makefile_lib_x11_software
	@echo "built make/output/makefile_lib_x11_software"

make/output/makefile_lib_x11_software:
	make/lib/gen/gen_x11.sh release software

makefile_lib_x11_egl: \
make/output/makefile_lib_x11_egl
	@echo "built make/output/makefile_lib_x11_egl"

make/output/makefile_lib_x11_egl:
	make/lib/gen/gen_x11.sh release egl

makefile_lib_x11_vulkan: \
make/output/makefile_lib_x11_vulkan
	@echo "built make/output/makefile_lib_x11_vulkan"

make/output/makefile_lib_x11_vulkan:
	make/lib/gen/gen_x11.sh release vulkan

## named makefile targets for development library builds
makefile_development_lib_x11_software:
	make/lib/gen/gen_x11.sh development software

makefile_development_lib_x11_egl:
	make/lib/gen/gen_x11.sh development egl

makefile_development_lib_x11_vulkan:
	make/lib/gen/gen_x11.sh development vulkan

## named makefile targets for sanitized library builds
makefile_sanitized_memory_lib_x11_software:
	make/lib/gen/gen_x11.sh sanitized_memory software

makefile_sanitized_memory_lib_x11_egl:
	make/lib/gen/gen_x11.sh sanitized_memory egl

makefile_sanitized_memory_lib_x11_vulkan:
	make/lib/gen/gen_x11.sh sanitized_memory vulkan

makefile_sanitized_undefined_lib_x11_software:
	make/lib/gen/gen_x11.sh sanitized_undefined software

makefile_sanitized_undefined_lib_x11_egl:
	make/lib/gen/gen_x11.sh sanitized_undefined egl

makefile_sanitized_undefined_lib_x11_vulkan:
	make/lib/gen/gen_x11.sh sanitized_undefined vulkan

makefile_sanitized_address_lib_x11_software:
	make/lib/gen/gen_x11.sh sanitized_address software

makefile_sanitized_address_lib_x11_egl:
	make/lib/gen/gen_x11.sh sanitized_address egl

makefile_sanitized_address_lib_x11_vulkan:
	make/lib/gen/gen_x11.sh sanitized_address vulkan

makefile_sanitized_thread_lib_x11_software:
	make/lib/gen/gen_x11.sh sanitized_thread software

makefile_sanitized_thread_lib_x11_egl:
	make/lib/gen/gen_x11.sh sanitized_thread egl

makefile_sanitized_thread_lib_x11_vulkan:
	make/lib/gen/gen_x11.sh sanitized_thread vulkan

# simple example makefiles targets
## named & regular makefile targets for release simple example builds
makefile_example_simple_x11_software: \
make/output/makefile_example_simple_x11_software
	@echo "built make/output/makefile_example_simple_x11_software"

make/output/makefile_example_simple_x11_software:
	make/example/simple/gen/gen_x11.sh release software

makefile_example_simple_x11_egl: \
make/output/makefile_example_simple_x11_egl
	@echo "built make/output/makefile_example_simple_x11_egl"

make/output/makefile_example_simple_x11_egl:
	make/example/simple/gen/gen_x11.sh release egl

makefile_example_simple_x11_vulkan: \
make/output/makefile_example_simple_x11_vulkan
	@echo "built make/output/makefile_example_simple_x11_vulkan"

make/output/makefile_example_simple_x11_vulkan:
	make/example/simple/gen/gen_x11.sh release vulkan

## named makefile targets for development simple example builds
makefile_development_example_simple_x11_software:
	make/example/simple/gen/gen_x11.sh development software

makefile_development_example_simple_x11_egl:
	make/example/simple/gen/gen_x11.sh development egl

makefile_development_example_simple_x11_vulkan:
	make/example/simple/gen/gen_x11.sh development vulkan

## named makefile targets for sanitized simple example builds
makefile_sanitized_memory_example_simple_x11_software:
	make/example/simple/gen/gen_x11.sh sanitized_memory software

makefile_sanitized_memory_example_simple_x11_egl:
	make/example/simple/gen/gen_x11.sh sanitized_memory egl

makefile_sanitized_memory_example_simple_x11_vulkan:
	make/example/simple/gen/gen_x11.sh sanitized_memory vulkan

makefile_sanitized_undefined_example_simple_x11_software:
	make/example/simple/gen/gen_x11.sh sanitized_undefined software

makefile_sanitized_undefined_example_simple_x11_egl:
	make/example/simple/gen/gen_x11.sh sanitized_undefined egl

makefile_sanitized_undefined_example_simple_x11_vulkan:
	make/example/simple/gen/gen_x11.sh sanitized_undefined vulkan

makefile_sanitized_address_example_simple_x11_software:
	make/example/simple/gen/gen_x11.sh sanitized_address software

makefile_sanitized_address_example_simple_x11_egl:
	make/example/simple/gen/gen_x11.sh sanitized_address egl

makefile_sanitized_address_example_simple_x11_vulkan:
	make/example/simple/gen/gen_x11.sh sanitized_address vulkan

makefile_sanitized_thread_example_simple_x11_software:
	make/example/simple/gen/gen_x11.sh sanitized_thread software

makefile_sanitized_thread_example_simple_x11_egl:
	make/example/simple/gen/gen_x11.sh sanitized_thread egl

makefile_sanitized_thread_example_simple_x11_vulkan:
	make/example/simple/gen/gen_x11.sh sanitized_thread vulkan

# complex example makefiles targets
## named & regular makefile targets for release complex example builds
makefile_example_complex_x11_software: \
make/output/makefile_example_complex_x11_software
	@echo "built make/output/makefile_example_complex_x11_software"

make/output/makefile_example_complex_x11_software:
	make/example/complex/gen/gen_x11.sh release software

makefile_example_complex_x11_egl: \
make/output/makefile_example_complex_x11_egl
	@echo "built make/output/makefile_example_complex_x11_egl"

make/output/makefile_example_complex_x11_egl:
	make/example/complex/gen/gen_x11.sh release egl

makefile_example_complex_x11_vulkan: \
make/output/makefile_example_complex_x11_vulkan
	@echo "built make/output/makefile_example_complex_x11_vulkan"

make/output/makefile_example_complex_x11_vulkan:
	make/example/complex/gen/gen_x11.sh release vulkan

## named makefile targets for development complex example builds
makefile_development_example_complex_x11_software:
	make/example/complex/gen/gen_x11.sh development software

makefile_development_example_complex_x11_egl:
	make/example/complex/gen/gen_x11.sh development egl

makefile_development_example_complex_x11_vulkan:
	make/example/complex/gen/gen_x11.sh development vulkan

## named makefile targets for sanitized complex example builds
makefile_sanitized_memory_example_complex_x11_software:
	make/example/complex/gen/gen_x11.sh sanitized_memory software

makefile_sanitized_memory_example_complex_x11_egl:
	make/example/complex/gen/gen_x11.sh sanitized_memory egl

makefile_sanitized_memory_example_complex_x11_vulkan:
	make/example/complex/gen/gen_x11.sh sanitized_memory vulkan

makefile_sanitized_undefined_example_complex_x11_software:
	make/example/complex/gen/gen_x11.sh sanitized_undefined software

makefile_sanitized_undefined_example_complex_x11_egl:
	make/example/complex/gen/gen_x11.sh sanitized_undefined egl

makefile_sanitized_undefined_example_complex_x11_vulkan:
	make/example/complex/gen/gen_x11.sh sanitized_undefined vulkan

makefile_sanitized_address_example_complex_x11_software:
	make/example/complex/gen/gen_x11.sh sanitized_address software

makefile_sanitized_address_example_complex_x11_egl:
	make/example/complex/gen/gen_x11.sh sanitized_address egl

makefile_sanitized_address_example_complex_x11_vulkan:
	make/example/complex/gen/gen_x11.sh sanitized_address vulkan

makefile_sanitized_thread_example_complex_x11_software:
	make/example/complex/gen/gen_x11.sh sanitized_thread software

makefile_sanitized_thread_example_complex_x11_egl:
	make/example/complex/gen/gen_x11.sh sanitized_thread egl

makefile_sanitized_thread_example_complex_x11_vulkan:
	make/example/complex/gen/gen_x11.sh sanitized_thread vulkan

# library binaries targets
## targets for software backends
build_lib_x11_software: \
make/output/makefile_lib_x11_software
	$(MAKE) -f make/output/makefile_lib_x11_software

## targets for opengl backends
build_lib_x11_egl: \
make/output/makefile_lib_x11_egl
	$(MAKE) -f make/output/makefile_lib_x11_egl

## targets for vulkan backends
build_lib_x11_vulkan: \
make/output/makefile_lib_x11_vulkan
	$(MAKE) -f make/output/makefile_lib_x11_vulkan

# simple example binaries targets
## targets for software simple example binaries
build_example_simple_x11_software: \
make/output/makefile_lib_x11_software \
make/output/makefile_example_simple_x11_software
	$(MAKE) -f make/output/makefile_lib_x11_software
	$(MAKE) -f make/output/makefile_lib_x11_software headers
	$(MAKE) -f make/output/makefile_example_simple_x11_software

## targets for opengl simple example binaries
build_example_simple_x11_egl: \
make/output/makefile_lib_x11_egl \
make/output/makefile_example_simple_x11_egl
	$(MAKE) -f make/output/makefile_lib_x11_egl
	$(MAKE) -f make/output/makefile_lib_x11_egl headers
	$(MAKE) -f make/output/makefile_example_simple_x11_egl

## targets for vulkan simple example binaries
build_example_simple_x11_vulkan: \
make/output/makefile_lib_x11_vulkan \
make/output/makefile_example_simple_x11_vulkan
	$(MAKE) -f make/output/makefile_lib_x11_vulkan
	$(MAKE) -f make/output/makefile_lib_x11_vulkan headers
	$(MAKE) -f make/output/makefile_example_simple_x11_vulkan

# complex example binaries targets
## targets for software complex example binaries
build_example_complex_x11_software: \
make/output/makefile_lib_x11_software \
make/output/makefile_example_complex_x11_software
	$(MAKE) -f make/output/makefile_lib_x11_software
	$(MAKE) -f make/output/makefile_lib_x11_software headers
	$(MAKE) -f make/output/makefile_example_complex_x11_software

## targets for opengl complex example binaries
build_example_complex_x11_egl: \
make/output/makefile_lib_x11_egl \
make/output/makefile_example_complex_x11_egl
	$(MAKE) -f make/output/makefile_lib_x11_egl
	$(MAKE) -f make/output/makefile_lib_x11_egl headers
	$(MAKE) -f make/output/makefile_example_complex_x11_egl

## targets for vulkan complex example binaries
build_example_complex_x11_vulkan: \
make/output/makefile_lib_x11_vulkan \
make/output/makefile_example_complex_x11_vulkan
	$(MAKE) -f make/output/makefile_lib_x11_vulkan
	$(MAKE) -f make/output/makefile_lib_x11_vulkan headers
	$(MAKE) -f make/output/makefile_example_complex_x11_vulkan

# simple example run targets
## targets for software simple example run
run_example_simple_x11_software: \
make/output/makefile_lib_x11_software \
make/output/makefile_example_simple_x11_software
	$(MAKE) -f make/output/makefile_lib_x11_software
	$(MAKE) -f make/output/makefile_lib_x11_software headers
	$(MAKE) -f make/output/makefile_example_simple_x11_software run

## targets for opengl simple example run
run_example_simple_x11_egl: \
make/output/makefile_lib_x11_egl \
make/output/makefile_example_simple_x11_egl
	$(MAKE) -f make/output/makefile_lib_x11_egl
	$(MAKE) -f make/output/makefile_lib_x11_egl headers
	$(MAKE) -f make/output/makefile_example_simple_x11_egl run

## targets for vulkan simple example run
run_example_simple_x11_vulkan: \
make/output/makefile_lib_x11_vulkan \
make/output/makefile_example_simple_x11_vulkan
	$(MAKE) -f make/output/makefile_lib_x11_vulkan
	$(MAKE) -f make/output/makefile_lib_x11_vulkan headers
	$(MAKE) -f make/output/makefile_example_simple_x11_vulkan run

# complex example run targets
## targets for software complex example run
run_example_complex_x11_software: \
make/output/makefile_lib_x11_software \
make/output/makefile_example_complex_x11_software
	$(MAKE) -f make/output/makefile_lib_x11_software
	$(MAKE) -f make/output/makefile_lib_x11_software headers
	$(MAKE) -f make/output/makefile_example_complex_x11_software run

## targets for opengl complex example run
run_example_complex_x11_egl: \
make/output/makefile_lib_x11_egl \
make/output/makefile_example_complex_x11_egl
	$(MAKE) -f make/output/makefile_lib_x11_egl
	$(MAKE) -f make/output/makefile_lib_x11_egl headers
	$(MAKE) -f make/output/makefile_example_complex_x11_egl run

## targets for vulkan complex example run
run_example_complex_x11_vulkan: \
make/output/makefile_lib_x11_vulkan \
make/output/makefile_example_complex_x11_vulkan
	$(MAKE) -f make/output/makefile_lib_x11_vulkan
	$(MAKE) -f make/output/makefile_lib_x11_vulkan headers
	$(MAKE) -f make/output/makefile_example_complex_x11_vulkan run

# simple example leak targets
## targets for software simple example leak
leak_example_simple_x11_software: \
make/output/makefile_lib_x11_software \
make/output/makefile_example_simple_x11_software
	$(MAKE) -f make/output/makefile_lib_x11_software
	$(MAKE) -f make/output/makefile_lib_x11_software headers
	$(MAKE) -f make/output/makefile_example_simple_x11_software leak

## targets for opengl simple example leak
leak_example_simple_x11_egl: \
make/output/makefile_lib_x11_egl \
make/output/makefile_example_simple_x11_egl
	$(MAKE) -f make/output/makefile_lib_x11_egl
	$(MAKE) -f make/output/makefile_lib_x11_egl headers
	$(MAKE) -f make/output/makefile_example_simple_x11_egl leak

## targets for vulkan simple example leak
leak_example_simple_x11_vulkan: \
make/output/makefile_lib_x11_vulkan \
make/output/makefile_example_simple_x11_vulkan
	$(MAKE) -f make/output/makefile_lib_x11_vulkan
	$(MAKE) -f make/output/makefile_lib_x11_vulkan headers
	$(MAKE) -f make/output/makefile_example_simple_x11_vulkan leak

# complex example leak targets
## targets for software complex example leak
leak_example_complex_x11_software: \
make/output/makefile_lib_x11_software \
make/output/makefile_example_complex_x11_software
	$(MAKE) -f make/output/makefile_lib_x11_software
	$(MAKE) -f make/output/makefile_lib_x11_software headers
	$(MAKE) -f make/output/makefile_example_complex_x11_software leak

## targets for opengl complex example leak
leak_example_complex_x11_egl: \
make/output/makefile_lib_x11_egl \
make/output/makefile_example_complex_x11_egl
	$(MAKE) -f make/output/makefile_lib_x11_egl
	$(MAKE) -f make/output/makefile_lib_x11_egl headers
	$(MAKE) -f make/output/makefile_example_complex_x11_egl leak

## targets for vulkan complex example leak
leak_example_complex_x11_vulkan: \
make/output/makefile_lib_x11_vulkan \
make/output/makefile_example_complex_x11_vulkan
	$(MAKE) -f make/output/makefile_lib_x11_vulkan
	$(MAKE) -f make/output/makefile_lib_x11_vulkan headers
	$(MAKE) -f make/output/makefile_example_complex_x11_vulkan leak
