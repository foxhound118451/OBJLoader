#pragma once
#define MODEL_DATA_H

#include <vector>
#include <math.h>
#include <material.h>
#include <string>
#include <unordered_map>

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
    std::string material; //material path 
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    float smoothing = 0;
    unsigned int VAO = 0;
};

struct ModelData
{
    std::string path; //path of model
    std::vector<MeshData> meshes;
    std::unordered_map<std::string, Material> materials;
};


//prototypes;
ModelData load_obj(const char* path);
