# **OBJ Loader**

Wavefront .obj 3d model file loader and viewer.

Viewer builds on the loader, the loader itself is independent.<br/>
Each are split into their own directories.

## Usage
./viewer/OBJViewer [path] <br/>
(I'll get it to work on Windows someday.)

## Compilation
Install & link listed dependencies, build using CMake.

### Build Dependencies
[NanoGUI](https://github.com/mitsuba-renderer/nanogui), [GLFW 3.4 Core](https://www.glfw.org), and [GLM](https://github.com/g-truc/glm). <br/>
[GLAD2](https://github.com/Dav1dde/glad) and [STB_IMAGE](https://github.com/nothings/stb) are included in source.
