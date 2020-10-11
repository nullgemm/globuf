# Globox

## Contexts backends details
X11
 - `software` provides CPU and GPU buffers using XCB's SHM module.
 - `egl` provides OpenGL and OpenGL ES contexts using XCB and EGL
   (which relies on GLX/libX11 in this case).
   Transparency is not supported because of a bug in EGL.
 - `glx` provides OpenGL contexts using XCB and GLX/libX11.
 - `vk` provides Vulkan contexts using Vulkan's WSI
   It is not yet implemented.

Wayland
 - `software` provides an abstract buffer using libwayland-client
 - `egl` provides OpenGL and OpenGL ES contexts using only EGL
 - `vk` provides Vulkan contexts using Vulkan's WSI
   It is not yet implemented.

Windows
 - `software` provides an abstract buffer using GDI
 - `egl` provides OpenGL contexts using an internal WGL wrapper
   It is not yet implemented.
 - `wgl` provides OpenGL contexts using only WGL
 - `vk` provides Vulkan contexts using Vulkan's WSI
   It is not yet implemented.

Mac OS
 - `software` provides an abstract buffer using CoreImage
   (which relies on gallium3D software OpenGL rendering)
 - `egl` provides OpenGL ES contexts using ANGLE
   (which implements OpenGL ES on top of metal)
   It is not yet implemented.
 - `vk` provides Vulkan contexts using MoltenVK
   (which implements Vulkan on top of metal)
   It is not yet implemented.

## Compiling
### Wayland
Globox supports Plasma's background blur Wayland protocol, and although we will
obviously stay compatible with compositors which do not support this protocol,
it is required to install its .xml files in order to compile.

By making this mandatory we make sure the feature is available to Plasma users,
without having to build a specific binary for that purpose.

Under ArchLinux, Plasma's protocols can be installed using the 
`plasma-wayland-protocols` package found in the `Extra` repo.

The official protocols repository can be found on
[invent.kde.org](https://invent.kde.org/libraries/plasma-wayland-protocols).
