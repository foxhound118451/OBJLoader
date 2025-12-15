#pragma once
#define MATERIAL_H
#include <vector>
#include <string>

struct Color
{
	float r = 0.0f;
	float g = 0.0f;
	float b = 0.0f;
};

/*
* .mtl Material information
*/
struct Material
{
	char name[64]{}; //taken from 'newmtl' statement 
        char path[256]; //path of material
	Color ambient; //'Ka' ambient 
	Color diffuse; //'Kd' diffuse
	Color specular; //'Ks' specular
	Color transmission_filter; //'Tf' transmission filter
	float specular_pow; //'Ns' specular exponent 
	float optical_density; //'Ni' index of refraction
	float dissolve; //'d' opacity
	char ambient_map[256]{}; //'map_Ka' ambient map path
	char diffuse_map[256]{}; //'map_Kd' diffuse map path
	char specular_map[256]{}; //'map_Ks' specular map path
	unsigned char illumination; //'illum'/'illum_#' illumination model
};

/*
* Return vector of Materials from file
*/
std::vector<Material> parse_mtl(const char* path);
