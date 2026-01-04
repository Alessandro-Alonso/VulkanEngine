# [NETA Engine]

![Vulkan](https://img.shields.io/badge/Vulkan-1.4-red) ![C++](https://img.shields.io/badge/C++-20-blue) ![License](https://img.shields.io/badge/License-MIT-green)

> Note: This is an educational project developed to learn the Vulkan API and real-time rendering concepts. It is a work in progress.

## Overview

[NETA Engine] is a custom game engine written in C++ and Vulkan. The goal of this project is to understand the low-level details of modern graphics APIs, including memory management, synchronization, and the graphics pipeline.

The project is currently in the initial "Hello Triangle" stage. The core Vulkan boilerplate has been implemented to render basic geometry.

**Implemented:**
* **Initialization:** Vulkan Instance, Debug Messenger, and Physical Device selection.
* **Device Setup:** Logical Device creation and Queue families.
* **Presentation:** Window surface (GLFW) and Swapchain support.
* **Pipeline:** Graphics Pipeline creation, Render Passes, and Shader Module management.
* **Rendering:** Command Buffer recording and Frame synchronization (Semaphores & Fences).

**Roadmap (Upcoming):**
* [ ] Vertex Buffers (to move geometry data out of shaders)
* [ ] Staging Buffers (for high-performance memory transfer)
* [ ] Uniform Buffers (for MVP matrices)
* [ ] Texture Mapping
* [ ] 3D Model Loading

## Tech Stack

* Language: C++ (Standard [e.g., C++17 or C++20])
* Graphics API: Vulkan SDK
* Windowing: [e.g., GLFW]
* Math: [e.g., GLM]
* Build System: CMake

## Resources & Credits

This project was built with the help of the following resources:
* Vulkan Tutorial by Alexander Overvoorde
* Vulkan Guide by Victor Blanco
* Sascha Willems' Examples
* [Vulkan 1.3 Specification](https://registry.khronos.org/vulkan/specs/1.3-extensions/html/vkspec.html) - The official technical manual from Khronos.
* [RenderDoc Documentation](https://renderdoc.org/docs/index.html) - Essential for debugging frames and analyzing the pipeline.
* [NVIDIA Vulkan Best Practices](https://developer.nvidia.com/blog/vulkan-dos-donts/) - Advanced optimization techniques for real-world engines.
* [Vulkan in 30 Minutes](https://renderdoc.org/vulkan-in-30-minutes.html) - A dense technical summary for experienced graphics programmers.

## License

Distributed under the MIT License. See LICENSE for more information.
