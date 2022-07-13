# Globox
Globox is a windowing library designed over the course of several years,
as a way for me to learn to use the four major display systems:
Wayland, X11, DWM (Windows) and Quartz (macOS).

## Globox history
### The old design
The very first design of Globox intended it as a single-thread library that
supports asynchronous event handling without using timer-based event pumps to
deal with the modal loops issues impacting Windows and macOS in that case.

My goal was to prove it is possible to build a fast windowing library
abstraction with existing system libraries without relying on parallelism and
without resorting to this horrible event pump hack to work around a minor issue.

This design was succesfully implemented, but it comes with its own limits. Most
notably, the techniques used to achieve this goal break desktop integration
under Windows and macOS. Along the way, I also realized it is probably not a
very good idea to fight a system's intended design. It often results in weird
specificities the user is not expecting, which is not what UI programmers want.

### The new design
For these reasons, the current design of Globox diverges dramatically from the
old one, and is much more similar to what already exists in other open-source
windowing libraries like the SDL.

This new architecture intends Globox as fundamentally multi-threaded, with event
handling and rendering taking place on different threads, and nothing tampering
with modal loops since they can't block the app, being on a dedicated thread.

### What's left of the dream
While the architecture of Globox changed, the philosophy behind it did not.
Globox was built around the following opinions:
 - all users deserve a qualitative experience whatever their opinions or choices
 - any of the programs we write should only do one thing and do it well
 - the main purpose of code is to communicate between programmers
Which gives us the following key principles:
 - apps built using Globox should be truly multi-platform.
 - the library should be modular and minimalistic.
 - the code should be clear and simple

## Using globox

## Modifying globox
### Adding a new backend
### Adding a new platform
