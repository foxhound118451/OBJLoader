#ifndef SHADER_H
#define SHADER_H

#if defined(NANOGUI_GLAD)
    #if defined(NANOGUI_SHARED) && !defined(GLAD_GLAPI_EXPORT)
        #define GLAD_GLAPI_EXPORT
    #endif

    #include <glad/gl.h>
#else
    #if defined(__APPLE__)
        #define GLFW_INCLUDE_GLCOREARB
    #else
        #define GL_GLEXT_PROTOTYPES
    #endif
#endif

#include <nanogui/nanogui.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <fstream>
#include <sstream>
#include <iostream>
#include <string>

using namespace std;

//compile shader program given vertex and fragment shader paths, return shader ID
class Shader
{
public:
    unsigned int ID;
    Shader (const char* vertPath, const char* fragPath)
    {
        //read vertex shader
        string vertexCode;
        string fragmentCode;
        ifstream vFile;
        ifstream fFile;

        vFile.exceptions(ifstream::badbit);
        fFile.exceptions(ifstream::badbit);
        try
        {
            vFile.open(vertPath);
            fFile.open(fragPath);
            stringstream vStream, fStream;
            vStream << vFile.rdbuf();
            fStream << fFile.rdbuf();
            vFile.close();
            fFile.close();
            vertexCode = vStream.str();
            fragmentCode = fStream.str();
        }
        catch (ifstream::failure e)
        {
            std::cout << "ERROR SHADER FILE AT PATH: '" << vertPath << "'NOT SUCCESSFULLY READ, REASON: " << e.what() << endl;
        }
        const char* vShaderCode = vertexCode.c_str();
        const char* fShaderCode = fragmentCode.c_str();

        char info[512];
        int success;

        //compile vertex shader
        unsigned int vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex, 1, &vShaderCode, NULL);
        glCompileShader(vertex);
        glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(vertex, 512, NULL, info);
            std::cout << "ERROR: VERTEX SHADER COMPILATION FAILED, PATH: '" << vertPath << "'.\n" << info << endl;
        }

        //compile fragment shader
        unsigned int fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, &fShaderCode, NULL);
        glCompileShader(fragment);
        glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(fragment, 512, NULL, info);
            std::cout << "ERROR: FRAGMENT SHADER COMPILATION FAILED, PATH: '" << fragPath << "'\n" << info << endl;
        }

        //link shader program
        unsigned int program = glCreateProgram();
        glAttachShader(program, vertex);
        glAttachShader(program, fragment);
        glLinkProgram(program);
        glGetProgramiv(program, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(program, 512, NULL, info);
            std::cout << "ERROR: SHADER PROGRAM LINKING FAILED\n" << info << endl;
        }
        glDeleteShader(vertex);
        glDeleteShader(fragment);
        ID = program;
    }
    void setInt(const string&name, int val)
    {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), val);
    }
    void setFloat(const string &name, float val)
    {
        glUniform1f(glGetUniformLocation(ID, name.c_str()), val);
    }
    void setMat4(const string &name, glm::mat4& mat)
    {
        glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }
    void setVec3(const string& name, glm::vec3 vec)
    {
        glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &vec[0]);
    }
    void setVec3(const string& name, float x, float y, float z)
    {
        glm::vec3 vec(x,y,z);
        glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &vec[0]);
    }
    void use()
    {
        glUseProgram(ID);
    }
};
#endif
