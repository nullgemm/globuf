# Todo
 - support dpishit in windows window creation to be dpi-aware

# Cleanup
 o x11_sw (uses XCB to access a raw buffer on CPU or GPU)
 o x11_gl (uses EGL for OpenGL and glES)
 - x11_vk (uses Vulkan's API)

 - way_sw (uses libwayland-client to access an abstract buffer)
 - way_gl (uses EGL for OpenGL and glES)
 - way_vk (uses Vulkan's API)

 - win_sw (uses GDI to access an abstract buffer)
 - win_gl (uses EGL for OpenGL and glES)
 - win_vk (uses Vulkan's API)

 - osx_sw (uses Apple's CoreImage, which depends on gallium3D software OpenGL implementation)
 - osx_gl (uses EGL - actually ANGLE, which implements gLES 2.0 and 3.0 on top of metal)
 - osx_vk (uses MoltenVK, which implements Vulkan on top of metal)

# New platforms
 - dfb_swr (uses gbm / dri)
 - dfb_gl (uses EGL for OpenGL and glES)
 - dfb_vk (uses Vulkan's API)

 - android_swr (uses a simple glES texture)
 - android_gl (uses EGL for glES)
 - android_vk (uses Vulkan's API)
