---OBJ Loader---

Reads Wavefront .obj files, and returns data in a format 
usable for OpenGL rendering.

Slow and not completely functional yet. Currently can only parse one 
object group, and does not yet support materials.

---Functions---

    --Mesh loadObjFromPath(const char* path)--

    Parses .obj file at path, initializes OpenGL buffers & vertex attributes,
    returns Mesh struct containing VAO, vertex/indice buffers.

    --void drawMesh(Mesh mesh);

    Uses VAO, vertices, & indices in Mesh to draw. Shader must be set up by
    user.
