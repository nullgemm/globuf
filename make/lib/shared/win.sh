#!/bin/bash

ar -x globuf_win_common.a
ar -x globuf_win_software.a
ar -x ../globuf_pe.a

x86_64-w64-mingw32-gcc -shared -o globuf_win_software.dll *.o -lshcore -lgdi32 -ldwmapi
