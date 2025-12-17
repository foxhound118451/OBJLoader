#pragma once
#define MODEL_H 
#include "model_data.h"
#include <vector>

struct Model 
{
    ModelData data;
    std::string name;
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
void draw_model(Model model);
