#define NANOGUI_GLAD
#if defined(NANOGUI_GLAD)
    #if defined(NANOGUI_SHARED) && !defined(GLAD_GLAPI_EXPORT)
        #define GLAD_GLAPI_EXPORT
    #endif

    #include <glad/gl.h>
#else
    #if defined(__APPLE__)
        #define GLFW_INCLUDE_GLCOREARB
    #else
        #define GL_GLEXT_PROTOTYPES
    #endif
#endif

#include <GLFW/glfw3.h>
#include <nanogui/nanogui.h>
#include <vector>
#include "model.h"
#include "texture.h"
#include "shader.h"

/**
 * Load mesh's data into appropriate OpenGL buffers,
 * initialize buffer objects & texures, then store IDs in given mesh.
**/
inline Mesh load_mesh(MeshData data, std::unordered_map<std::string, Material> materials, std::unordered_map<std::string, unsigned int>& textures)
{
    Mesh mesh;
    mesh.data = data;

    unsigned int VAO = 0;
    unsigned int VBO = 0;
    unsigned int EBO = 0;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);		
    
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

    glBufferData(GL_ARRAY_BUFFER, data.vertices.size() * sizeof(Vertex), data.vertices.data(), GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, data.indices.size() * sizeof(unsigned int), data.indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_TRUE, sizeof(Vertex), (void*)0); //position
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_TRUE, sizeof(Vertex), (void*)(offsetof(Vertex, tx))); //texcoord
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_TRUE, sizeof(Vertex), (void*)(offsetof(Vertex, nx))); //normal

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    //assign VAO ID
    mesh.VAO = VAO;
    
    //retrieve materials and parse texture paths 
    if (auto search = materials.find(data.material); search != materials.end())
    {
        Material mat = (*search).second;
        if(*mat.ambient_map)
        {
            if(auto search = textures.find(mat.ambient_map); search != textures.end())
            {
                mesh.ambient_map = search->second;
            }
            else
            {
                mesh.ambient_map = load_texture(mat.ambient_map);
                textures.insert({mat.ambient_map, mesh.ambient_map});
            }
        }
        if(*mat.diffuse_map)
        {
            if(auto search = textures.find(mat.diffuse_map); search != textures.end())
            {
                mesh.diffuse_map = search->second;
            }
            else
            {
                mesh.diffuse_map = load_texture(mat.diffuse_map);
                textures.insert({mat.diffuse_map, mesh.diffuse_map});
            }
        }
        if(*mat.specular_map)
        {
            if(auto search = textures.find(mat.specular_map); search != textures.end())
            {
                mesh.specular_map = search->second;
            }
            else
            {
                mesh.specular_map = load_texture(mat.specular_map);
                textures.insert({mat.specular_map, mesh.specular_map});
            }
        }
    }
    else if (data.material.size())
    {
        std::cerr << "ERROR: Failed retrieving material: '" << mesh.data.material << "'." << std::endl;
    }
    return mesh;
}

Model load_model(ModelData model_data)
{
    Model model;

    model.scale_factor = model_data.scale_factor;
    model.path = model_data.path;
    //get model name from path
    std::string dir = model_data.path; 
    size_t slash_pos = dir.find_last_of("/") + 1;
    size_t dot_pos = dir.find_last_of(".");
    std::string name = dir.substr(slash_pos, dot_pos);
    model.name = name;

    model.materials = model_data.materials;
    //load meshes and add to counters
    int size = model_data.meshes.size();
    for (int i = 0; i < size; ++i)
    {
        MeshData mesh_data = model_data.meshes.at(i);
        Mesh mesh = load_mesh(mesh_data, model_data.materials, model.textures);
        model.meshes.push_back(mesh);
        model.vertice_count += mesh_data.vertices.size();
        model.indice_count += mesh_data.indices.size();
        model.material_count += (mesh_data.material.size() ? 1 : 0);
    }

    return model;
}

/**
 * Draw all meshes in model.
**/
void draw_model(Model model, Shader shader)
{
    unsigned int size = model.meshes.size();
    for (int i = 0; i < size; ++i)
    {
        Mesh mesh = model.meshes.at(i);
        Material mat;
        if (auto search = model.materials.find(mesh.data.material); search != model.materials.end())
        {
            mat = (Material) search->second;
        }
        if (mesh.ambient_map)
        {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, mesh.ambient_map);
            shader.setInt("ambient_map", 0);
            shader.setInt("has_ambient_map", 1);
        }
        if (mesh.diffuse_map)
        {
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, mesh.diffuse_map);
            shader.setInt("diffuse_map", 1);
            shader.setInt("has_diffuse_map", 1);
        }
        if (mesh.specular_map)
        {
            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, mesh.specular_map);
            shader.setInt("specular_map", 2);
            shader.setInt("has_specular_map", 1);
        }

        shader.setVec3("ambient_mod", mat.ambient.r, mat.ambient.g, mat.ambient.b);
        shader.setVec3("diffuse_mod", mat.diffuse.r, mat.diffuse.g, mat.diffuse.b);
        shader.setVec3("specular_mod", mat.specular.r, mat.specular.g, mat.specular.b);
        glBindVertexArray(mesh.VAO);
        glDrawElements(GL_TRIANGLES, mesh.data.indices.size(), GL_UNSIGNED_INT, 0);
    }
}
