#pragma once
#define MATERIAL_H
#include <vector>

struct Color
{
	float r;
	float g;
	float b;
};

/*
* .mtl Material information
*/
struct Material
{
	char* name; //'newmtl'
	Color ambient; //'Ka' ambient 
	Color diffuse; //'Kd' diffuse
	Color specular; //'Ks' specular
	Color transmission_filter; //'Tf' transmission filter
	float specular_pow; //'Ns' specular exponent 
	float optical_density; //'Ni' index of refraction
	float dissolve; //'d' opacity
	char* ambient_map; //'map_Ka' ambient map path
	char* diffuse_map; //'map_Kd' diffuse map path
	char* specular_map; //'map_Ks' specular map path
	unsigned char illumination; //'illum'/'illum_#' illumination model
};

/*
* Return vector of Materials from file
*/
std::vector<Material> parse_mtl(const char* path);
