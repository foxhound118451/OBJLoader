# **OBJ Loader**

Wavefront .obj 3d model file parser and viewer.

Very unfinished, only supports a small subset of the .obj spec.<br/>

Currently parses vertices, normals, and faces. Faces are converted into tris, and 
resulting vertices/indices are stored in a Mesh struct. Larger files will cause 
it to hang.

## Compilation
Install & link listed dependencies, build using CMake.

# Dependencies
Link GLFW 3.4, and GLM.
GLAD2 and STB_IMAGE are used, but are included in source.
