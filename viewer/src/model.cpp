#include "model.h"
#include "glad/gl.h"
#include <vector>

/**
 * Load mesh's data into appropriate OpenGL buffers.
**/
inline void load_mesh(MeshData& mesh)
{
    unsigned int VAO = 0;
    unsigned int VBO = 0;
    unsigned int EBO = 0;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);		
    
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

    glBufferData(GL_ARRAY_BUFFER, mesh.vertices.size() * sizeof(Vertex), mesh.vertices.data(), GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.indices.size() * sizeof(unsigned int), mesh.indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_TRUE, sizeof(Vertex), (void*)0); //position
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_TRUE, sizeof(Vertex), (void*)(offsetof(Vertex, tx))); //texcoord
    
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_TRUE, sizeof(Vertex), (void*)(offsetof(Vertex, nx))); //normal

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    mesh.VAO = VAO;
}

Model load_model(ModelData model_data)
{
    Model model;

    //get model name from path
    std::string dir = model_data.path; 
    size_t slash_pos = dir.find_last_of("/") + 1;
    size_t dot_pos = dir.find_last_of(".");
    std::string name = dir.substr(slash_pos, dot_pos);
    model.name = name;

    //load meshes and add to counters
    int size = model_data.meshes.size();
    for (int i = 0; i < size; ++i)
    {
        MeshData mesh = model_data.meshes.at(i);
        load_mesh(mesh); //assigns VAO index in struct
        model_data.meshes.at(i) = mesh;
        model.vertice_count += mesh.vertices.size();
        model.indice_count += mesh.indices.size();
        model.material_count += (mesh.material.size() ? 1 : 0);
    }

    model.data = model_data;
    return model;
}

/**
 * Draw all meshes in model.
**/
void draw_model(Model model)
{
    ModelData data = model.data;
    unsigned int size = data.meshes.size();
    for (int i = 0; i < size; ++i)
    {
        MeshData mesh = data.meshes.at(i);
        glBindVertexArray(mesh.VAO);
        glDrawElements(GL_TRIANGLES, mesh.indices.size(), GL_UNSIGNED_INT, 0);
    }
}
