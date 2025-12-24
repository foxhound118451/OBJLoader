# **OBJ Loader**

Wavefront .obj 3d model file loader and viewer.

The viewer is dependent on the loader, loader is independent.<br/>

## Usage
Currently only tested on Linux.

./viewer/OBJViewer [path] [size].<br/>

Size sets the model's scale.

## Compilation
Install & link listed dependencies, build using CMake.

# Build Dependencies
Link [NanoGUI](https://github.com/mitsuba-renderer/nanogui), [GLFW 3.4 Core](https://www.glfw.org), and [GLM](https://github.com/g-truc/glm).
[GLAD2](https://github.com/Dav1dde/glad) and [STB_IMAGE](https://github.com/nothings/stb) are used, but are included in source.
