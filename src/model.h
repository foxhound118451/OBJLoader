#pragma once

#include <vector>
#include <math.h>

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

struct Mesh
{
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	unsigned int VAO = -1;
};


//prototypes;
void drawMesh(Mesh mesh);
Mesh loadObjFromPath(const char* path);
