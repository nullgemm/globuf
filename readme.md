![globox_macos_linux_windows](https://user-images.githubusercontent.com/5473047/144758425-9d5e4433-3670-41b2-82f9-6ced93d69468.png)
*cross-platform background blur under macOS, Windows and Linux (X11/Wayland)*

# Globox
Globox is a cross-platform windowing library for Linux, Windows and macOS.
It was built as a lightweight and modular alternative to similar libraries.
The entire library is written in C99 without the help of any other language.

Only window management is taken care of, for more interactivity you will have
to set the user input callback. Inside you can write a simple event handler or
call another cross-platform library implementing advanced input mechanisms, like
[Willis](https://github.com/nullgemm/willis).
Willis handles the keyboad and mouse and should support any keymap wizardry.
Its main function can be used directly as a Globox input callback if the
keyboard and mouse combo is all you care about.

The modularity of Globox is a key part of its architecture: even window handling
and context handling were carefully separated, making it possible to implement a
custom context type very easily without looking at the window creation code.

## Contexts backends details
X11
 - `software` provides CPU and GPU buffers using XCB's SHM module.
 - `egl` provides OpenGL and OpenGL ES contexts using XCB and EGL
   (which relies on GLX/libX11 in this case).
   Transparency is not supported because of a bug in EGL.
 - `glx` provides OpenGL contexts using XCB and GLX/libX11.
 - `vk` provides Vulkan contexts using Vulkan's WSI
   (It is not implemented yet)

Wayland
 - `software` provides an abstract buffer using libwayland-client
 - `egl` provides OpenGL and OpenGL ES contexts using only EGL
 - `vk` provides Vulkan contexts using Vulkan's WSI
   (It is not implemented yet)

Windows
 - `software` provides an abstract buffer using GDI
 - `egl` provides OpenGL contexts using EGLproxy
   (which is a simple WGL wrapper)
   Transparency is not supported because of a bug in EGLproxy
 - `wgl` provides OpenGL contexts using only WGL
 - `vk` provides Vulkan contexts using Vulkan's WSI
   (It is not implemented yet)

mac OS
 - `software` provides an abstract buffer using CoreImage
   (which relies on gallium3D software OpenGL rendering)
 - `egl` provides OpenGL ES contexts using ANGLE
   (which implements OpenGL ES on top of metal)
 - `vk` provides Vulkan contexts using MoltenVK
   (which implements Vulkan on top of metal)
   (It is not implemented yet)

## Example
It is possible to generate simple makefiles to build egl, wgl, glx & software
examples without decorations using the scripts in `make/example/simple/gen`.
A small desktop application using Globox, Willis, DPIshit, Cursoryx and
providing an example implementation of client-side window decorations
can be built using the scripts in `make/example/complex/gen`.
For the desktop example you will need to clone with `--recurse-submodules`.
Alternatively, get the submodules by running the following commands:
```
git submodule init
git submodule update
```

## Compiling
Generate the makefiles you need using the scripts in `make/lib/gen/`.
Then, simply run the makefiles created at the root of the repository.
```
make -f makefile_BACKEND_CONTEXT_NATIVE
```
You can also run the scripts in `make/lib/release/` to directly build
releases without any intervention.

### Wayland
Globox supports Plasma's background blur Wayland protocol, and although we will
obviously stay compatible with compositors which do not support this protocol,
it is required to install its .xml files in order to compile. By making this
mandatory we make sure the feature is available to Plasma users, without having
to build a specific binary for that purpose.

Under ArchLinux, Plasma's protocols can be installed using the 
`plasma-wayland-protocols` package found in the `Extra` repo.
The official protocols repository can be found on
[invent.kde.org](https://invent.kde.org/libraries/plasma-wayland-protocols).

### macOS
#### Cross-compiling from Linux
To get a working macOS toolchain under Linux we recommend cloning the
[instant_macos_sdk](https://github.com/nullgemm/instant_macos_sdk)
repo and following the instructions in the readme.

After the toolchain was installed and your environment variables updated you
will also need to install objcopy, which is required by the generated makefile.

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

The generated makefile also requires some extra tools, starting with `objcopy`
```
brew install binutils
```

After installation, the binary can be found under
```
/usr/local/Cellar/binutils/VERSION_NUMBER/bin/objcopy
```

#### Nota Bene
Another tool required is `objconv`. Since it can't be installed easily,
the generated makefile will download and compile a local version automatically.
In a similar fashion, the `metalANGLE` library used to provide metal-backed
glES support is fetched by the `gen` script from an official release archive.

An extra script is available in the `make` folder in case you want to use
the original `ANGLE`: it suffers from small visual glitches but is the official
version of the library, in which `metalANGLE` changes are eventually integrated.
The `ANGLE` binaries are extracted from the latest `chromium` release.

### Windows
#### Cross-compiling from Linux
To cross-compile the Windows platform from Linux you will need to install MinGW.
Globox was designed to be Wine-compatible so you can use it to run the examples
(but remember Wine is not Windows and does not support transparency or blur).

#### Compiling from Windows
The Windows backend can also be compiled from Windows for easier testing.
*Do not download Visual Studio just for Globox, it would be completely overkill!*
You only need builds of the POSIX `rm`, `mv` and `cp` and GNU's `make` and `objcopy`.

We recommend using the
[Git for Windows SDK](https://github.com/git-for-windows/build-extra/releases/latest)
to get a basic POSIX environment without cluttering your Windows installation:
it will create an isolated MSYS2 with a lot of useful stuff already configured.

Make sure to get the "SDK" version from the link above as the releases from
[git-scm.com](https://git-scm.com)
do not include all the make-dependencies and come without a package manager.
We also recommend that you replace the command-line font with a
[decent one](https://dejavu-fonts.github.io/Download.html).

The Git for Windows SDK provides you with gcc, which can be used to compile.
When going that way, use the `gen_windows_mingw.sh` script to generate a makefile.

Alternatively, you can find Microsoft's *stuff* in the
[Build Tools for Visual Studio](https://visualstudio.microsoft.com/downloads/#build-tools-for-visual-studio-2019).
The setup is done through the Visual Studio Installer and starts automatically.
If you miss some components just start it again and click "Modify" to add them.
Nothing is installed by default, you need to open the "individual components" tab
and select those you want. In our case, this will be a compiler and SDK:
 - `Windows 10 SDK`
 - `MSVC - VS C++ x64/x86 build tools`
 - `MSVC - VS C++ x64/x86 Spectre-mitigated libs`

The `gen_windows_msvc.sh` scripts use powershell to detect the version number
of these tools automatically without relying on Visual Studio, so make sure
it is available on your system as well before generating makefiles.

## Testing
The `ci` folder contains dockerfiles and scripts to generate testing images
and can be used locally, but a `concourse_pipeline.yml` file is also available
here and should be usable with a few modifications in case you want a more
user-friendly experience. Our own Concourse instance will not be made public
since it runs on a home server (mostly for economic reasons).

## Known issues
The following issues are known and can only be fixed with platform-specific code
 - `Wayland` Mouse release events are not sent after interactive move & resize |
   All tested compositors seem to emit a dummy mouse movement event instead, so
   the only way out is to treat those received after initiating an interactive
   move or resize as a mouse release, and ask Globox to exit interactive mode.
 - `macOS` The GL ES viewport is limited to the bottom-left quarter |
   For some reason using a retina display makes macOS return fake sizes,
   expressed in logic pixels twice the size of the actual pixels.
   The only valid fix is to get this scaling value from Globox.

The following issues are known and, to our knowledge, simply cannot be fixed
 - `Windows 11` Resizing is slower than on `Windows 10` |
   Profiling with [VerySleepy](https://github.com/VerySleepy/verysleepy) reveals
   the slowest part of the complex example to be Microsoft's function
   responsible for resizing windows...
 - `Windows` Snap-Drag mechanisms are not available |
   Microsoft does not expose an API to control Snap-Drag,
   and the hack we use to provide a consistent event-loop behaviour across
   platforms prevents us from getting access to it normally.
 - `macOS` Magnetic window positioning is not available |
   Apple does not expose an API to control magnetic window positioning,
   and the hack we use to provide a consistent event-loop behaviour across
   platforms prevents us from getting access to it normally.
