#include <glad/gl.h>
#include <limits.h>
#include <GLFW/glfw3.h>
#include <model_data.h>
#include <material.h>
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <string>
#include <string.h>
#include <stdexcept>

using namespace std;

struct input_buffers
{
    vector<float> vertices;
    vector<float> tex_coords;
    vector<float> normals;
    vector<unsigned int> indices;
};

struct output_buffers
{
    vector<MeshData> meshes;
    unordered_map<string, Material> materials;
};


//prototypes
static int parse_obj_file(const char* file_path, output_buffers* out_buf);
static void parse_vertex(char* line, vector<float>* vertices);
static inline void parse_texcoord(char* line, vector<float>* texcoords);
static void parse_face(char* line, input_buffers input_buffers, MeshData& out_buf, unordered_map<string, unsigned int> *vertex_map);
static inline unsigned int parse_face_vertex(char* face_vertex, input_buffers& input_buffers, MeshData& out_buf, unordered_map<string, unsigned int>* vertex_map);
static vector<unsigned int> fan_triangulate(vector<unsigned int>* face);

//global vars
char* obj_path = NULL;

/*
	Parse Wavefront .obj file into ModelData struct
	Path is relative to resources/models
*/
ModelData load_obj(const char* path)
{

    output_buffers *out_buf = new output_buffers;
    //if (!parse_obj_file(path, output_buffers))
    parse_obj_file(path, out_buf);
    //{
        //parse faces, use indices to create final vertex & indice array.


    ModelData model;
    model.path = path;
    model.materials = out_buf->materials;
    int size = out_buf->meshes.size();
    for (int i = 0; i < size; ++i)
    {
        MeshData mesh = out_buf->meshes.at(i);
        model.meshes.push_back(mesh);
    }
    delete out_buf;
    return model;

    //}
    //else
    //{
            //cout << "Failed parsing .obj file at path: " << path << endl;
            //return mesh;
    //}
}

/*
* Parse .obj file at path and store vertices/tex_coords/normals/faces into given vectors. Return 0 on success, -1 on error.
*/
static int parse_obj_file(const char *file_path, output_buffers* output_buffers)
{		
    ifstream file = ifstream(file_path);
    obj_path = (char*)file_path;
    if (!file){
        cout << "ERROR: Failed opening .obj file at path: " << file_path << endl;
        return -1;
    }

    vector<Vertex> vertices;
    MeshData mesh;
    unordered_map<string, unsigned int> vertex_map;
    input_buffers input_buffers;

    /*
    * First, parse vertices, vertex texture coordinates, vertex normal coordinates into separate arrays.
    * Then, parse faces (indices of vertex/vertex texture/vertex normal).  
    * Use fan triangulation to convert any polygonal face into triangles
    *
    * Next, copy vertex indices from faces into indice array;
    * Then, read faces, grab indices of vertex texture coordinates, & vertex normals,
    * combine them with corresponding vertex in new vertex array.
    */
    while (!(file.eof()  || file.fail())) //parse line by line
    {
        std::string line_str;
        getline(file, line_str);

        size_t len = line_str.size();
        char line[len + 1];
        strcpy (line, line_str.c_str());
        if (line[len-1] == '\r') //remove carriage ret
        {
            line[len-1] = '\0';
        }

        //read all vertices, texture coordinates, normals into corresponding buffers,
        //read faces into buffer
        //parse face buffer, new vertex array with corresponding texture coordinates/normals

        switch (line[0])
        {
            case 'v': //vertex
            {
                switch (line[1])
                {
                    case 't': //vertex texture
                        parse_texcoord(line, &input_buffers.tex_coords);
                        break;
                    case 'n': //vertex normal
                        parse_vertex(line, &input_buffers.normals);
                        break;
                    case ' ': //vertex
                        parse_vertex(line, &input_buffers.vertices);
                        break;
                }
                break;
            }
            case 'o': //group/object
            case 'g':
            {
                //get name
                char* strtok_state{};
                strtok_r(line, " ", &strtok_state);
                string name_str = strtok_r(NULL, " ", &strtok_state);

                if(!mesh.name.empty())//previous mesh exists
                {
                    //push prev mesh
                    output_buffers->meshes.push_back(mesh);
                    //clear mesh data
                    MeshData new_mesh; 
                    mesh = new_mesh;
                    vertex_map.clear();
                }

                mesh.name = name_str;

                break;
            }

            case 'f': //face
            {
                parse_face(line, input_buffers, mesh, &vertex_map);
                break;
            }
            case 'm': //material file declaration
            {
                strtok(line, " ");

                string dir = obj_path;
                size_t pos = dir.find_last_of("/"); //get directory of model file
                dir = dir.substr(0, pos);

                char mtl_path[128];
                strcat(mtl_path, dir.c_str());
                strcat(mtl_path, "/");
                strcat (mtl_path, strtok(NULL, " "));
                vector<Material> materials = parse_mtl(mtl_path);

                int len = materials.size();
                //map materials to their names so meshes dont store entire material
                for (auto& it : materials)
                {
                    Material material = (Material) it;
                    output_buffers->materials.insert({material.name, material});
                }
                break;
            }
            case 'u': //set material of current mesh
            {
                char* strtok_state{};
                strtok_r(line, " ", &strtok_state);
                char* material = strtok_r(NULL, " ", &strtok_state);
                string material_str = material;
                mesh.material = material_str;
                break;
            }
        }
    }
    output_buffers->meshes.push_back(mesh); //last mesh in file
    file.close();
    if (file.bad())
    {
        cerr << "ERROR: Failed reading from file at path: " << file_path << endl;
        return -1;
    }
    else
    {
        return 0;
    }
}
static inline void parse_texcoord(char* line, vector<float>* texcoords)
{
    char* strtok_state{};
    char* texcoord = strtok_r(line, " ", &strtok_state);

    texcoord = strtok_r(NULL, " ", &strtok_state);
    (*texcoords).push_back(stof(texcoord));
    texcoord = strtok_r(NULL, " ", &strtok_state);
    (*texcoords).push_back(stof(texcoord));
}

/*
* Parse vertex/vertex normal into given vector
*/
inline void parse_vertex(char* line, vector<float> *vertices)
{
    char* strtok_state{};
    int strmax = 0;
    char* vertice = strtok_r(line, " ", &strtok_state);
    int i = 0;

    vertice = strtok_r(NULL, " ", &strtok_state);
    (*vertices).push_back(stof(vertice));
    vertice = strtok_r(NULL, " ", &strtok_state);
    (*vertices).push_back(stof(vertice));
    vertice = strtok_r(NULL, " ", &strtok_state);
    (*vertices).push_back(stof(vertice));
}

/*
* Parse line with face (sequence of v/vt/vn indices).
* Converts face into Vertex (v/vt/vn) and indice pointing to Vertex
*
* For each Vertex defined in face, check if already mapped,
* if already mapped, set indice to indice of Vertex
* else append vertex buffer with new vertex (v/vt/vn) and set indice to vertex
*/
static void parse_face(char* line, input_buffers input_buffers, MeshData& out_buf, unordered_map<string, unsigned int>* vertex_map)
{
    char* strtok_state{};
    char* face_vertex = strtok_r(line, " ", &strtok_state); // v/vt/vn vertex indice set (only parsing vertex indice so far)
    vector<unsigned int> face;

    while ((face_vertex = strtok_r(NULL, " ", &strtok_state))) //parse vertex indices separated by spaces
    {
            face.push_back(parse_face_vertex(face_vertex, input_buffers, out_buf, vertex_map));
    }

    //fan triangulate vertex indices so that any polygon becomes tri
    vector<unsigned int> indices = fan_triangulate(&face);
    out_buf.indices.insert(out_buf.indices.end(), indices.begin(), indices.end());
}

/*
* Parse string of vertex indices  
*
*/
static inline unsigned int parse_face_vertex(char* face_vertex, input_buffers& input_buffers, MeshData& out_buf, unordered_map<string, unsigned int> *vertex_map)
{
    string face_vertex_copy = face_vertex;
    const auto i = vertex_map->find(face_vertex);
    if (i == vertex_map->end())
    {
        //vertex indices not yet parsed
        unsigned int cur_index = (out_buf.vertices).size();//index of current vertex

        vertex_map->insert({ face_vertex_copy, cur_index }); //create new entry pointing to current index in vertex buffer

        //parse indices from line
        unsigned int v = 0;
        unsigned int vt = INT_MAX;
        unsigned int vn = INT_MAX;

        char* strtok_state{};
        char* indice_str{};

        if ((indice_str = strtok_r(face_vertex, "/", &strtok_state)))
        {
            v = stoi(indice_str) - 1; //make index start at 0
        }
        if ((indice_str = strtok_r(NULL, "/", &strtok_state)))
        {
            vt = stoi(indice_str) - 1;
        }
        if ((indice_str = strtok_r(NULL, "/", &strtok_state)))
        {
            vn = stoi(indice_str) - 1;
        }
        //create vertex using indicesCylinder
        Vertex vertex;
        //pos
        vertex.x = input_buffers.vertices.at(v * 3);
        vertex.y = input_buffers.vertices.at((v * 3) + 1);
        vertex.z = input_buffers.vertices.at((v * 3) + 2);
        //texcoord
        //if (vt != INT_MAX)
        //{ 
                //vertex.tx = input_buffers.tex_coords.at(vt * 2);
                //vertex.ty = input_buffers.tex_coords.at((vt * 2) + 1);
        //}
        //normal
        if (vn != INT_MAX)
        {
            vertex.nx = input_buffers.normals.at(vn * 3);
            vertex.ny = input_buffers.normals.at((vn * 3) + 1);
            vertex.nz = input_buffers.normals.at((vn * 3) + 2);
        }
        //push vertex to final buffer
        out_buf.vertices.push_back(vertex);
        return cur_index;
    }
    else
    {
        unsigned int indice = vertex_map->at(face_vertex);
        return(indice);
    }
}

/*
* Use fan triangulation to split polygon indices into triangles
*/
static vector<unsigned int> fan_triangulate(vector<unsigned int>* face)
{
    //use fan triangulation to split face into tris
    //0 1 2 3 ... into 0 1 2, 0 2 3, 0 3 4 ...
    int len = face->size();
    int i = 0;
    vector<unsigned int> indices;

    while (i+2 < len)
    {
        (indices).push_back(face->at(0));
        (indices).push_back(face->at(i + 1));
        (indices).push_back(face->at(i + 2));
        ++i;
    }
    return indices;
}
