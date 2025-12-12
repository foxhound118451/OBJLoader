# **OBJ Loader**

Reads Wavefront .obj files, returns data as a Mesh struct,
usable for OpenGL rendering.

Currently, it only supports a small subset of the .obj spec.<br/>

For now, it parses vertices, normals, and faces. Faces are converted into tris, 
resulting the vertexes/indices are stored in one Mesh. Larger files will cause 
it to hang.

## Functions

### Mesh loadObjFromPath(const char* path)

Parses .obj file at path, initializes OpenGL buffers & vertex attributes,
returns Mesh struct containing VAO, vertex/indice buffers.

### void drawMesh(Mesh mesh)

Uses mesh buffers from loadObjFromPath to draw. Does not set up shader.

## Compilation
Install dependencies, build using CMake.

#Dependencies
Link GLFW 3.4, GLAD (Core 3.3), GLM.
