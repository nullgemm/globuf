#!/bin/bash

# get in the right folder
path="$(pwd)/$0"
folder=$(dirname "$path")
cd "$folder"/../.. || exit

echo "compiling GLSL shaders with glslangValidator..."
cd res/shaders/vk1
glslangValidator --target-env vulkan1.0 square_glsl_vk1.frag -o square_frag_vk1.spv
glslangValidator --target-env vulkan1.0 square_glsl_vk1.vert -o square_vert_vk1.spv
