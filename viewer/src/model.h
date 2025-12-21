#pragma once
#define MODEL_H 
#include <vector>
#include "model_data.h"
#include "shader.h"

struct Mesh
{
    MeshData data;
    unsigned int VAO = 0;
    unsigned int ambient_map = 0;
    unsigned int diffuse_map = 0;
    unsigned int specular_map = 0;
};

struct Model 
{
    std::vector<Mesh> meshes;
    std::unordered_map<std::string, Material> materials;
    std::string name;
    std::string path;
    unsigned int vertice_count = 0; //store counts
    unsigned int indice_count = 0;
    unsigned int material_count = 0;
};

/*
 * Initialize meshes using model_data, load into Model struct and return.
*/
Model load_model(ModelData model_data);

/*
 * Draw all meshes in model 
*/
void draw_model(Model model, Shader shader);
