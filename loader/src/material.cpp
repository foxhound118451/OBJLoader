#include <material.h>
#include <string.h>
#include <iostream>
#include <fstream>

using namespace std;

//Prototypes
int parse_material(Material* material);
static Color parse_color(char* line);
static inline char* fix_newline(std::string line);
static void parse_map(Material *material, char* line);

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

        char* strtok_state{};
        int i = 0;
        char *linetype{};
        if (linetype = strtok_r(line, " ", &strtok_state)) //skip empty lines
        {
            if (!strcmp(linetype, "newmtl")) //material declaration
            {
                //parse material
                Material* material = new Material;

                char *name = strtok_r(NULL, " ", &strtok_state);
                strcpy(material->name, name);
                if(parse_material(material)) //parse body of material statement
                {
                    materials.push_back(*material);
                    delete material;
                }
                if(!parse_material(material)) //parse body of material statement
                {
                    cerr << "ERROR: Failed parsing material with name: \"" << material->name << "\"" << endl;
                    //free material
                    file->close();
                    delete file;
                    delete material;
                    return materials;
                }
            }
            else if (line[0] != '#' && line[0] != '\r') //skip comments and carriage ret, throw error if not comment or material
            {
                cerr << "ERROR: Invalid argument in mtl file, with line '" << line_str << "'." << endl;
                file->close();
                delete file;
                return materials;
            }
        }
    }
    if (file->bad())
    {
        cerr << "ERROR: Failed reading material file." << endl;
    }
    file->close();
    delete file;
    return materials;
}

/*
* Parse lines until error, EOF, or material declaration
*/
int parse_material(Material* material)
{
    char *line;
    do
    {
        std::string line_str;
        std::getline(*file, line_str);
        
        size_t len = line_str.size();
        char line[len + 1];
        strcpy(line, line_str.c_str());
        if (line[len - 1] == '\r')
        {
            line[len - 1] = '\0';
        }

        int i = 0;
        if (line[0] == '\t') //skip tabs
        {
            i = 1;
        }

        switch (line[i])
        {
            case 'K': //color arg
                switch (line[i+1])
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
            case 'N': 
                switch (line[i+1])
                {
                    case 's': //specular exponent
                        material->specular_pow = stof(line+3);
                        break;
                }
                break;
            case 'm': //texture map statement 'map_xx'
                parse_map(material, line);
                break;
        }
    }
    while (!(file->eof() || file->bad() || *line == 'n'));
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

/*
 * Assign texture map path to appropriate place in Material struct.
*/
static void parse_map(Material *material, char* line)
{
    char* strtok_state{};
    strtok_r(line, "map_", &strtok_state);
    char* type = strtok_r(NULL, "map_", &strtok_state);
    char* path = NULL;

    switch(type[0])
    {
        case 'K':
            switch(type[1])
            {
                case 'a':
                    path = strtok_r(NULL, " ", &strtok_state);
                    strcpy(material->ambient_map, path);
                    break;
                case 'd':
                    path = strtok_r(NULL, " ", &strtok_state);
                    strcpy(material->diffuse_map, path);
                    break;
                case 's':
                    path = strtok_r(NULL, " ", &strtok_state);
                    strcpy(material->specular_map, path);
                    break;
            }
            break;
    }
}
