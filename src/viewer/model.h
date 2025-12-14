#pragma once
#define MODEL_H 
#include "model_data.h"
#include <vector>

struct Mesh
{
    std::string name;
    MeshData data;
    unsigned int VAO = 0;
};

struct Model 
{
    std::string name;
    std::vector<Mesh> meshes; 
    unsigned int vertices = 0; //store counts
    unsigned int indices = 0;
    unsigned int materials = 0;
};

/*
 * Initialize meshes using model_data, load into Model struct and return.
*/
Model load_model(ModelData model_data);

/*
 * Draw all meshes in model 
*/
void draw_model(Model model);
