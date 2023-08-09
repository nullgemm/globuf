# Globox
Globox is a portable windowing library for X11, Wayland, Windows and macOS.
It is entirely modular, with support for X11, Wayland, Windows and macOS
available in optional library binaries to be used with the Globox core.

Globox was built as a lightweight alternative to similar projects and only
provides windows, as demonstrated in the simple example, with the relevant
system libraries for only dependencies.

Input handling, cursor management and density detection must be taken care of 
using other libraries, such as Willis, Cursoryx and DPIshit, independent
projects usable with Globox as shown in the complex example.

This modular architecture makes it easy to add support for new platforms,
and also means it's possible to select the target window system at run time.

## Supported platforms and backends
Wayland
 - `software` provides an abstract buffer using libwayland-client. (TODO)
 - `vulkan` provides Vulkan contexts using Vulkan's WSI Wayland support. (TODO)
 - `egl` provides OpenGL contexts using only EGL and libwayland-client. (TODO)

X11
 - `software` provides CPU and GPU buffers using XCB's SHM module.
 - `vulkan` provides Vulkan contexts using Vulkan's WSI XCB support.
 - `egl` provides OpenGL contexts using XCB, EGL and libX11 when required.
   Transparent contexts are not supported yet because of a bug in EGL.
 - `glx` provides OpenGL contexts using XCB, GLX and libX11 when required.

Windows (background blur is not supported since it requires using private APIs)
 - `software` provides CPU buffers using GDI.
 - `vulkan` provides Vulkan contexts using Vulkan's WSI.
 - `wgl` provides OpenGL contexts using WGL and GDI.

macOS
 - `software` provides CPU buffers using layer-based views and CoreGraphics.
 - `vulkan` provides Vulkan contexts using moltenVK and metal layer-based views.
 - `egl` provides OpenGL ES contexts using ANGLE and metal layer-based views.


## Building
The build system for Globox consists in bash scripts generating ninja scripts.
The first step is to generate the ninja scripts for all the components needed.
Then, these ninja scripts must be ran to build the actual binaries.

### General steps for the library
You will usually need 3 modules to be able to use one of Globox's backend:
 - The Globox core, providing code common across platforms.
   This is what implements the main interface of the library,
   to which you will bind the backend of your choice at run time.
 - The Globox backend module, holding all code specific to a context type.
   This is what is actually executed when the interface is used,
   after you bind all the backend's functions to the main library.
 - The Globox platform module, containing code common across backends.
   This is here to avoid compiling common code into every backend module,
   so you can link multiple ones without having to worry about binary size.

All these components are generated using the scripts found in `make/lib`.
They take arguments: execute them alone to get some help about that.

The scripts named after executable file formats generate ninja scripts
to compile the Globox core module. An example use for Linux would be:
```
./make/lib/elf.sh development common
```

The scripts named after platforms will generate ninja scripts to compile
Globox platforms when supplied the `common` argument, but using backend
names instead will generate ninja scripts to compile backends:
```
./make/lib/x11.sh development common
./make/lib/x11.sh development vulkan
```

All ninja scripts are generated in `make/output`. To compile, simply execute
them using the original `ninja` or the faster `samurai` implementation.
```
ninja -f ./make/output/lib_elf.ninja
ninja -f ./make/output/lib_x11_common.ninja
ninja -f ./make/output/lib_x11_vulkan.ninja
```

All the binaries we build are automatically copied in a new `globox_bin` folder
suffixed with the latest tag on the repository, like this: `globox_bin_v0.0.0`.
To copy the library headers here and make the build ready to use and distribute,
we run the core and platform ninja scripts again, adding the `headers` argument:
```
ninja -f ./make/output/lib_elf.ninja headers
ninja -f ./make/output/lib_x11_vulkan.ninja headers
```

### General steps for the examples
To build the included examples, you must first compile the relevant modules
and have them and their headers available in the `globox_bin` folder.

Then, use the scripts in `make/examples` to generate the ninja scripts for
the example and backend you want. For the simple X11 Vulkan example:
```
./make/example/simple/x11.sh development vulkan
```

Execute the ninja script, and an example will be compiled in the `build` folder:
```
ninja -f ./make/output/example_simple_x11_vulkan.ninja
```

You may have noticed there are two variants of the examples: simple and complex.
Because Globox only implements windowing, the simple examples don't do much.

Since my other libraries implementing desktop-app features also needed examples,
I decided although they are completely independent to create a complex variant
integrating them together with Globox.

To compile these complex examples, you will therefore need these libraries.
For convenience, a script downloading their latest releases can be found in
```
./make/scripts/prepare.sh
```

You can then proceed to building the complex variant of the examples.


### Wayland support
(TODO)


### X11 support
Globox was built using the modern libxcb X11 library instead of libX11.
Make sure all its components are installed before you start compiling.


### Windows support
Our build system relies on the MinGW toolchain to build Windows binaries.

#### Compiling from Windows
To compile the Windows module under Windows, we recommend using the MinGW
toolchain provided by the [MSYS2](https://www.msys2.org) building platform.

For increased comfort we also recommend using Microsoft's "Windows Terminal":
it is available for download outside of the Microsoft Store on the project's
[GitHub](https://github.com/microsoft/terminal/latest).
It is possible to use MSYS2 from the new Windows Terminal with a custom profile:
to do this click the downwards arrow next to the tabs in the terminal window:
this will open the settings menu, from which new profiles can be created.
Create a new empty profile and paste the following command in the field for the
executable path - make sure the arguments are here otherwise it won't work.
```
C:/msys64/msys2_shell.cmd -defterm -here -no-start -ucrt64
```
You can also set the icon path to the following
```
C:/msys64/ucrt64.ico
```
Once your profile is configured, you can open new tabs using it from the main
window, or set it as the default profile from the settings menu.

Whether you decide to go for the Windows terminal or the included MinTTY,
make sure you are using MSYS2 with its UCRT64 environment, this is important.
When your setup is ready, install a basic MinGW toolchain in MSYS2, like this:
```
pacman -S --needed base-devel mingw-w64-ucrt-x86_64-toolchain git ninja unzip
```

For vulkan, you will need the vulkan development package:
```
pacman -S mingw-w64-ucrt-x86_64-vulkan-devel
```

You can then proceed to the general build steps.


#### Cross-compiling from Linux
To cross-compile the Windows module under Linux, simply install MinGW and build.
Wine is fully supported, so you will be able to test the examples as expected,
but remember Wine is not Windows and does not support transparency or blur.


### macOS support
Our build system relies on the Xcode toolchain to build macOS binaries.

#### Compiling from macOS
To compile the macOS module under macOS, we recommend that you don't download
the entirety of Xcode, as it would be overkill. Instead, simply get the
command-line tools using your favorite installation method.

If you do not want to register an Apple account, it is possible to get the bare
macOS SDK from Apple's "Software Update" servers using some of the scripts in
[instant_macos_sdk](https://github.com/nullgemm/instant_macos_sdk).

If you did not get Xcode, you also have to install git independently
```
brew install git
```

You can then proceed to the general build steps.


#### Cross-compiling from Linux
To cross-compile the macOS module under Linux, we recommed using OSXcross.
The toolchain can be deployed easily without an Apple account by cloning
[instant_macos_sdk](https://github.com/nullgemm/instant_macos_sdk)
and following the instructions in the readme.

You can then proceed to the general build steps.


### All-in-one helper script
For convenience, a helper script can be found in `make/scripts/build.sh`.
It will automatically build the library and an example in a single command line.
To use it, you must supply all the following arguments:
 - a build type
 - a platform name
 - a backend name
 - an example type
 - a build toolchain type

For instance:
```
./make/scripts/build.sh development x11 vulkan simple native
```


## Testing
### CI
The `ci` folder contains dockerfiles and scripts to generate testing images
and can be used locally, but a `concourse_pipeline.yml` file is also available
here and should be usable with a few modifications in case you want a more
user-friendly experience. Our own Concourse instance will not be made public
since it runs on a home server (mostly for economic reasons).


### Complex example
The complex example can help us test advanced features of Globox
that require specific capabilities not provided by the library itself.

Press (but don't hold) the following keyboard keys to trigger various actions:
- 1 => restore the window
- 2 => minimize the window
- 3 => maximize the window 
- 4 => make the window fullscreen
- G => grab/ungrab the mouse
- M => cycle through some custom app-defined cursors
- N => set the waiting cursor
- S => start an app-initiated window move
- any key around S => start an app-initiated window resize

When moving the window, the current screen resolution and physical size
should be printed in the logs. Multi-display setups should work fine,
and monitor hot-plugging is also fully supported.

When pressing a key, its scancode in qwerty equivalents should be logged,
along with the actual character this key press should print given the
current keyboard layout. Keyboard layout changes are supported,
as well as composition, deadkeys...


## Special thanks
Nathan J. for being amazing.
