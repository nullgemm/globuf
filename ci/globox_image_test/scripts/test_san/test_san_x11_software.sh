#!/bin/bash

# clone the globox repo
git clone https://github.com/nullgemm/globox.git
cd ./globox || exit

# build lib
./make/lib/gen/gen_x11.sh sanitized software
make -f makefile_lib_x11_software

# copy lib
./make/lib/release/release_headers.sh
./make/lib/release/release_copy_x11_software.sh

# build example
./make/example/simple/gen/gen_x11.sh sanitized software static
make -f makefile_example_simple_x11_software
cd ./bin || exit

# run the tests
DISPLAY=:1
XAUTHORITY=$(pwd)/xauth
export DISPLAY
export XAUTHORITY

echo "# generate a cookie for xauthority"
touch "$XAUTHORITY"
xauth add :1 . "$(mcookie)"

echo "# start Xorg on a dummy video output"
Xorg :1 -noreset +extension GLX +extension RANDR +extension RENDER +extension MIT-SHM -logfile ./xdummy.log -config /scripts/xorg.conf &
echo "# wait for the X server to start"
sleep 10

echo "# start xfwm"
xfwm4 &
echo "# wait for the window manager to start"
sleep 10

echo "# start the globox example"
./globox_example_simple_x11_software &
echo "# wait for the globox example to start"
sleep 10

echo "# xdo: find the window ID for the globox example"
id=$(xdotool search --name globox)
echo "# xdo: make the window smaller"
xdotool windowsize --sync "$id" 250 250
echo "# xdo: make the window bigger"
xdotool windowsize --sync "$id" 500 500
echo "# xdo: minimize the window"
xdotool windowminimize --sync "$id"
echo "# xdo: show the window"
xdotool windowmap --sync "$id"
echo "# xdo: close the window"
xdotool windowclose "$id"
echo "# wait for the globox example process to end"
sleep 10

echo "# close the window manager"
pkill xfwm4
echo "# wait for the window manager process to end"
sleep 10

echo "# close the X server"
pkill Xorg
echo "# wait for the X server process to end"
sleep 10
