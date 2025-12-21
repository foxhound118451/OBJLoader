#include <material.h>
#include <string.h>
#include <iostream>
#include <fstream>

using namespace std;

//Prototypes
static inline int parse_material_element(std::string& line, const char* path, Material& material);
static inline Color parse_color(char* line);
static inline char* fix_newline(std::string line);
static inline void parse_map(char* line, Material& material);

ifstream *file;

vector<Material> parse_mtl(const char* path)
{
    vector<Material> materials;
    file = new ifstream(path);
    if (!file)
    {
        cerr << "ERROR: Failed opening material file at path: " << path << endl;
        delete file;
        return materials;
    }
    Material material;
    while (!(file->eof() || file->fail())) //parse by line
    {
        std::string line_str;
        std::getline(*file, line_str);
        size_t len = line_str.size();
        char line[len + 1];
        strcpy(line, line_str.c_str());
        if (line[len - 1] == '\r') //remove carriage ret
        {
            line[len - 1] = '\0';
        }

        int offset = 0;
        if (line[0] == '\t') //skip tabs
        {
            offset = 1;
        }

        char* strtok_state{};
        char *linetype{};
        linetype = strtok_r(line, " ", &strtok_state); //skip empty spaces
        if (linetype) //ignore empty lines
        {
            switch (*linetype)
            {
                case 'n': //material definition
                {
                    if (*material.name) //prev material exists
                    {
                        materials.push_back(material);
                        Material mat; //wipe material
                        material = mat;
                    }
                    char *name = strtok_r(NULL, " ", &strtok_state);
                    strcpy(material.name, name);
                    break;
                }
                default: 
                {
                    parse_material_element(line_str, path, material);
                    break;
                }
            }
        }
    }
    if (file->bad())
    {
        cerr << "ERROR: Failed reading material file." << endl;
    }
    else //push last material in file
    {
        materials.push_back(material);
    }
    file->close();
    delete file;
    return materials;
}

/*
 * Store any parsed element in corresponding var in Material struct.
 * Returns 1 on success, -1 on error.
*/
inline int parse_material_element(std::string& line_str, const char* path, Material& material)
{
    //parse line 
    char* strtok_state{};


    string dir(path);
    size_t pos = dir.find_last_of("/"); //get directory of mtl 
    dir = dir.substr(0, pos);
    char mtl_path[128]{}; 
    strcpy(mtl_path, dir.c_str());
    strcat(mtl_path, "/");
    strcpy(material.path, mtl_path); //copy material directory into struct

    int len = line_str.length();
    if (line_str[len - 1] == '\r')
    {
        line_str[len - 1] = '\0';
    }

    char* line = (char*) line_str.c_str();
    if (line[0] == '\t') //skip tabs
    {
        ++line;
    }
    switch (line[0])
    {
        case 'K': //color arg
            switch (line[1])
            {
                case 'a': //ambient
                    line = strtok(line, " ");
                    material.ambient = parse_color(strtok(NULL, " ")); 
                    break;
                case 'd': //diffuse
                    line = strtok(line, " ");
                    material.diffuse = parse_color(strtok(NULL, " "));
                    break;
                case 's': //specular
                    line = strtok(line, " ");
                    material.specular = parse_color(strtok(NULL, " "));
                    break;
            }
            break;
        case 'N': 
            switch (line[1])
            {
                case 's': //specular exponent
                    line = strtok(line, " ");
                    material.specular_pow = stof(strtok(NULL, " "));
                    break;
            }
            break;
        case 'm': //texture map statement 'map_xx'
            parse_map(line, material);
            break;
    }
    return 1;
}

/*
 * Parse of color arguments
 * color read as floats (R G B) separated by spaces.
 * If one arg, set all colors to value.
 * If 3 args, set colors to respective args.
*/
static inline Color parse_color(char* line)
{
    Color color = {0.0f, 0.0f, 0.0f};
    switch(sscanf(line, "%f %f %f", &color.r, &color.g, &color.b))
    {
        case 1:
        {
            color.g = color.r;
            color.b = color.r;
            break;
        }
        case 3:
        {
            break;
        }
        default:
        {
            std::cerr << "ERROR: Failed parsing color." << std::endl;
            break;
        }
    }
    return color;
}

/*
 * Assign texture map path to appropriate place in Material struct.
*/
static inline void parse_map(char* line, Material& material)
{
    char* strtok_state{};
    //parse type of map and path
    char type[128]{};
    char map_path[256]{}; 
    strcpy(map_path, material.path);
    char mat_path[256]{};
    sscanf (line, "%*[^_]_%s %s", type, mat_path); 
    strcat (map_path, mat_path);

    switch(type[0])
    {
        case 'K':
            switch(type[1])
            {
                case 'a':
                    strcpy(material.ambient_map, map_path);
                    break;
                case 'd':
                    strcpy(material.diffuse_map, map_path);
                    break;
                case 's':
                    strcpy(material.specular_map, map_path);
                    break;
            }
            break;
    }
}
