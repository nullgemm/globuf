# Globox
Globox is a cross-platform windowing library for Linux, Windows and macOS.
It was built as a lightweight and modular alternative to similar libraries.

Only window management is taken care of, for more interactivity you will have
to set the user input callback. Inside you can write a simple event handler or
call another cross-platform library implementing advanced input mechanisms, like
[Willis](https://github.com/nullgemm/willis).
Willis handles the keyboad and mouse and should support any keymap wizardry.
Its main function can be used directly as a Globox input callback if the
keyboard and mouse combo is all you care about.

The modularity of Globox is key to its architecture: even window handling and
context handling were carefully separated, making it possible to implement a
custom context type very easily without looking at the window creation code.

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
   It is not yet implemented.
 - `egl` provides OpenGL contexts using an internal WGL wrapper
   It is not yet implemented.
 - `wgl` provides OpenGL contexts using only WGL
   It is not yet implemented.
 - `vk` provides Vulkan contexts using Vulkan's WSI
   It is not yet implemented.

mac OS
 - `software` provides an abstract buffer using CoreImage
   (which relies on gallium3D software OpenGL rendering)
 - `egl` provides OpenGL ES contexts using ANGLE
   (which implements OpenGL ES on top of metal)
 - `vk` provides Vulkan contexts using MoltenVK
   (which implements Vulkan on top of metal)
   It is not yet implemented.

## Compiling
First configure your test build using the variables in the makefile.
You can also set the corresponding environment variables if you prefer.

`PLATFORM` must be set to one of the following
 - `WAYLAND`
 - `X11`
 - `WINDOWS`
 - `MACOS`

`CONTEXT` must be set to one of the following
 - `SOFTWARE`
 - `EGL`
 - `GLX` (X11 only, see previous section)
 - `WGL` (Windows only, see previous section)
 - `VULKAN`

`NATIVE` must be set to one of the following
 - `TRUE` when compiling for your native OS
 - `FALSE` when cross-compiling

### Wayland
Globox supports Plasma's background blur Wayland protocol, and although we will
obviously stay compatible with compositors which do not support this protocol,
it is required to install its .xml files in order to compile. By making this
mandatory we make sure the feature is available to Plasma users, without having
to build a specific binary for that purpose.

Under ArchLinux, Plasma's protocols can be installed using the 
`plasma-wayland-protocols` package found in the `Extra` repo. The official
protocols repository can be found on
[invent.kde.org](https://invent.kde.org/libraries/plasma-wayland-protocols).

### macOS
#### Cross-compiling from Linux
To get a working macOS toolchain under Linux we recommend cloning the
[instant_macos_sdk](https://github.com/nullgemm/instant_macos_sdk)
repo and following the instructions in the readme.

After the toolchain was installed and your environment variables updated you
will also need to install objcopy, which is required by the example makefile.

#### Compiling from macOS
The macOS backend can also be compiled from macOS for easier debugging.
*Do not download Xcode just for globox, it would be completely overkill!*
Simply get the command-line tools using your favorite installation method.

If you do not want to register an Apple account, it is possible to get the bare
macOS SDK from Apple's "Software Update" servers using some of the scripts in
[instant_macos_sdk](https://github.com/nullgemm/instant_macos_sdk).

If you did not get Xcode, you also have to install git independently
```
brew install git
```

The example makefile also requires some extra tools, starting with `objcopy`
```
brew install binutils
```

After installation, the binary can be found under
```
/usr/local/Cellar/binutils/VERSION_NUMBER/bin/objcopy
```

#### Nota Bene
Another tool required is `objconv`. Since it can't be installed easily,
the example makefile will download and compile a local version automatically.
Same for the `metalANGLE` library used to provide metal-backed glES support,
which the makefile will get from an official release archive.

An extra script is available in the `res/angle` folder in case you want to use
the original `ANGLE`: it suffers from small visual glitches but is the official
version of the library, in which `metalANGLE` changes are eventually integrated.
The `ANGLE` binaries are extracted from the latest `chromium` release.
