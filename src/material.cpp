#include <material.h>
#include <string.h>
#include <iostream>
#include <fstream>

using namespace std;

//Prototypes
int parse_material(Material* material);

char line[256]; //current line being read
ifstream file;

vector<Material> parse_mtl(const char* path)
{
	vector<Material> materials;
	file = ifstream(path);
	if (!file)
	{
		cerr << "ERROR: Failed opening material file at path: " << path << endl;
		return materials;
	}

	while (!(file.eof() || file.fail())) //parse by line
	{
		file.getline(line, 256);

		char* strtok_state{};

		char* line_type = strtok_r(line, " ", &strtok_state);
		if (strcmp(line_type, "newmtl"))
		{
			//parse material
			Material material{};
			if(parse_material(&material))
			{
				materials.push_back(material);
			}
			else
			{
				cerr << "ERROR: Failed parsing material with name: \"" << material.name << "\"" << endl;
				return materials;
			}
		}
		else if (line_type[0] != '#')
		{
			cerr << "ERROR: Invalid argument in mtl file." << endl;
			return materials;
		}
	}
}

/*
* Parse lines until EOF or another material declaration
*/
int parse_material(Material* material)
{
	file.getline(line, 256);
	return 1;
}
