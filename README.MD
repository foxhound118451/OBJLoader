# **OBJ Loader**

Reads Wavefront .obj files, returns data in a Mesh struct usable for OpenGL rendering.

Currently, it only supports a small subset of the .obj spec.<br/>

For now, it parses vertices, normals, and faces. Faces are converted into tris, and 
resulting vertices/indices are stored in one Mesh struct. Larger files will cause 
it to hang.

## Functions

### Mesh loadObjFromPath(const char* path)

Parses .obj file at path, initializes OpenGL buffers & vertex attributes,
returns Mesh struct containing VAO, vertex/indice buffers.

### void drawMesh(Mesh mesh)

Uses mesh buffers from loadObjFromPath to draw. User must provide shader 
before function call.

## Compilation
Install & link listed dependencies, build using CMake.

# Dependencies
Link GLFW 3.4, and GLM.
GLAD2 is used, but it's included with the source files.
