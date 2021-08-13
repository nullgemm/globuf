#!/bin/bash

git clone https://github.com/nullgemm/globox.git
cd ./globox

# cppcheck
cppcheck \
	--force \
	--enable=performance \
	--enable=portability \
	--std=c99 \
	src
