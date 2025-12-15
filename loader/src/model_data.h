#pragma once
#define MODEL_DATA_H

#include <vector>
#include <math.h>
#include <material.h>
#include <string>

//structs

/* 
* x,y,z positions
* texture coordinates
* normal coordinates
*/
struct Vertex
{
	float x;
	float y;
	float z;
	float tx = NAN; //initialize to NAN to check if coords already specified for given vertex
	float ty = NAN;
	float nx = NAN;
	float ny = NAN;
	float nz = NAN;
};

struct MeshData
{
    std::string name;
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::string material[256]; //material name
    float smoothing = 0;
};

struct ModelData
{
    std::string name;
    std::vector<MeshData> meshes;
    std::unordered_map<std::string, Material> materials;
};


//prototypes;
ModelData load_obj(const char* path);
