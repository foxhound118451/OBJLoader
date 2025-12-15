#include "model.h"
#include "glad/gl.h"
#include <vector>

inline Mesh load_mesh(MeshData mesh_data)
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

    glBufferData(GL_ARRAY_BUFFER, mesh_data.vertices.size() * sizeof(Vertex), mesh_data.vertices.data(), GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh_data.indices.size() * sizeof(unsigned int), mesh_data.indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_TRUE, sizeof(Vertex), (void*)0); //position
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_TRUE, sizeof(Vertex), (void*)(offsetof(Vertex, tx))); //texcoord
    
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_TRUE, sizeof(Vertex), (void*)(offsetof(Vertex, nx))); //normal

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    Mesh mesh = 
    {
        .name = mesh_data.name,
        .data = mesh_data,
        .VAO = VAO
    };
    return mesh;
}

Model load_model(ModelData model_data)
{
    Model model;
    model.name = model_data.name;

    int size = model_data.meshes.size();

    for (int i = 0; i < size; ++i)
    {
        MeshData mesh_data = model_data.meshes.at(i);
        Mesh mesh = load_mesh(mesh_data);
        model.meshes.push_back(mesh);
        model.vertices += mesh_data.vertices.size();
        model.indices += mesh_data.indices.size();
        model.materials += mesh_data.materials.size();
    }
    return model;
}

void draw_model(Model model)
{
    unsigned int size = model.meshes.size();
    for (int i = 0; i < size; ++i)
    {
        Mesh mesh = model.meshes.at(i);
        glBindVertexArray(mesh.VAO);
        glDrawElements(GL_TRIANGLES, mesh.data.indices.size(), GL_UNSIGNED_INT, 0);
    }
}
