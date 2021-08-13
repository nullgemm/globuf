#!/bin/bash

docker run --privileged globox_image_test "$@" &> log
