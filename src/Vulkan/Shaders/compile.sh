#!/bin/bash

mkdir -p ../../build/Vulkan/Shaders

glslc shader.vert -o ../../build/Vulkan/Shaders/vert.spv
glslc shader.frag -o ../../build/Vulkan/Shaders/frag.spv

echo "Shaders compiled."
