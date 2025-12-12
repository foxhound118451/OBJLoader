#include <material.h>
#include <string.h>
#include <iostream>
#include <fstream>

using namespace std;

//Prototypes
int parse_material(Material* material);
static Color parse_color(char* line);

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
        char* linetype = strtok_r(line, " ", &strtok_state);
        if (!strcmp(linetype, "newmtl")) //material declaration
        {
            //parse material
            Material material{.name = strtok_r(NULL, " ", &strtok_state)};
            if(parse_material(&material)) //parse body of material statement
            {
                materials.push_back(material);
            }
            else
            {
                cerr << "ERROR: Failed parsing material with name: \"" << material.name << "\"" << endl;
                return materials;
            }
        }
        else if (line[0] != '#') //skip comments, throw error if not comment or material
        {
            cerr << "ERROR: Invalid argument in mtl file, with line '" << line << "'." << endl;
            return materials;
        }
    }
    if (file.fail())
    {
        cerr << "ERROR: Failed reading material file." << endl;
        return materials;
    }
}

/*
* Parse lines until error, EOF, or material declaration
*/
int parse_material(Material* material)
{
    file.getline(line, 256);
    switch (*line)
    {
        case 'K': //color arg
            switch (line[1])
            {
                case 'a': //ambient
                    material->ambient = parse_color(line); 
                    break;
                case 'd': //diffuse
                    material->diffuse = parse_color(line);
                    break;
                case 's': //specular
                    material->specular = parse_color(line);
                    break;
            }
            break;
    }
    return 1;
}

/*
 * Parse line for color argument 
 * color read as floats (R G B) separated by spaces.
 * If one arg, set all colors to value.
 * If 3 args, set colors to respective args.
*/
static Color parse_color(char* line)
{
    Color color = {0.0f, 0.0f, 0.0f};
    char* strtok_state{};
    strtok_r(line, " ", &strtok_state);
    char* cur_color = strtok_r(NULL, " ", &strtok_state);
    color.r = stof(cur_color);
    if (cur_color = strtok_r(NULL, " ", &strtok_state))
    {
        color.g = stof(cur_color);
        if (cur_color = strtok_r(NULL, " ", &strtok_state))
        {
            color.b = stof(cur_color);
        }
        else
        {
            cerr << "ERROR: Invalid color format in mtl file." << endl;
        }
    }
    else
    {
        color.b = color.r;
        color.g = color.r;
    }
    return color;
}
