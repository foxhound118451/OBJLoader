/*
* Temporary file used for debugging model loader.
*/

#define NANOGUI_USE_OPENGL
#define NANOGUI_GLAD
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

#include <GLFW/glfw3.h>
#include <nanogui/nanogui.h>

#include <string>
#include <iostream>
#include <filesystem>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace glm;
using namespace std;

#include "shader.h"
#include "camera.h"
#include "model.h"

GLFWwindow* window = NULL;
nanogui::Screen* screen = NULL;

float cameraSensitivity = 100.0f;
float cameraSpeed = 10.0f;
float deltaTime = 0.0f;
float lastFrame = 0.0f;
double prev_x = 0.0;
double prev_y = 0.0;
int s_width = 1280;
int s_height = 720;
float angle = 0.0f;
float scale_num = 1.0f;

//prototypes
void processInput(GLFWwindow* window);
static void windowSizeCallback(GLFWwindow* window, int width, int height);
static void cursorPosCallback(GLFWwindow* window, double xpos, double ypos);

Camera camera(vec3(0.0, 2.0f, 3.0f), vec3(0.0, 1.0f, 0.0f));
int main(int argc, char* argv[])
{
    if (!glfwInit())
    {
        std::cout << "Failed to initialize GLFW." << endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    window = glfwCreateWindow(s_width, s_height, "OpenGL", NULL, NULL);

    if (!window)
    {
        cout << "Failed to create GLFW window" << endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

#if defined(NANOGUI_GLAD)
    if (!gladLoadGL(glfwGetProcAddress))
    {
        cout << "Failed to initialize GLAD" << endl;
        return -1;
    }
#endif

    //set callbacks
    glfwSetCursorPosCallback(window, cursorPosCallback);
    glfwSetWindowSizeCallback(window, windowSizeCallback);
    glfwSetFramebufferSizeCallback(window, [](GLFWwindow *, int width, int height) {
            screen->resize_callback_event(width,height);
            }
    );
    glfwSetMouseButtonCallback(window, [](GLFWwindow *, int button, int action, int mods) {
            screen->key_callback_event(button, action, action, mods);
            }
    );
    


    glViewport(0, 0, s_width, s_height);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    cout << "Path: " << filesystem::current_path() << endl;

    Shader shader = Shader("shaders/model.vert", "shaders/model.frag");
    shader.use();

    Shader light = Shader("shaders/model.vert", "shaders/light.frag");

    vec3 cameraPos = vec3(0.0f, 0.0f, 2.0f);
    vec3 lightPos = vec3(1.0f, 1.5f, 2.0f);




    Model obj;
    switch (argc)
    {
        case 3:
            scale_num = stof(argv[2]);
        case 2:
            obj = load_model(load_obj(argv[1]));
            break;
        default:
            std::cout << "Usage: OBJViewer [path]." << std::endl;
            return 1;
            break;
    }

    cout << "Read " << obj.vertice_count << " vertices, " << obj.indice_count << " indices." << endl;


    Model prism = load_model(load_obj("models/prism.obj"));

    prev_x = s_width / 2.0f;
    prev_y = s_height / 2.0f;

    //init nanogui screen
    screen = new nanogui::Screen();
    screen->initialize(window, true);
    //create nanogui gui
    bool enabled = true;
    nanogui::FormHelper *gui = new nanogui::FormHelper(screen);
    nanogui::ref<nanogui::Window> nanogui_window = gui->add_window(nanogui::Vector2i(10,10), "Form helper example");

    gui->add_group("Basic types");
    bool bvar = false;
    string strval;
    gui->add_variable("bool", bvar)->set_tooltip("Test tooltip");
    gui->add_variable("string", strval);
    screen->set_visible(true);
    screen->perform_layout();
    nanogui_window->center();



    while (!glfwWindowShouldClose(window))
    {
        glEnable(GL_DEPTH_TEST);
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        processInput(window);


        glViewport(0, 0, s_width, s_height);
        glClearColor(0.7f, 0.7f, 0.7f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        camera.updateCameraVectors();

        shader.use();
        mat4 view = camera.GetViewMatrix();
        shader.setMat4("view", view);
        mat4 projection = perspective(radians(75.0f), (float)s_width / (float)s_height, 0.1f, 100.0f);
        shader.setMat4("projection", projection);

        //lighting uniforms
        shader.setVec3("lightPos", lightPos);
        shader.setVec3("ambient", vec3(0.6f, 0.6f, 0.2f));

        //draw model
        mat4 model = mat4(1.0f);
        model = translate(model, vec3(0.0f, 2.0f, -3.0f));
        model = scale(model, vec3(scale_num));
        shader.setMat4("model", model);
        shader.setMat4("view", view);
        shader.setMat4("projection", projection);
        shader.setVec3("objectColor", 0.5, 0.5, 0.9);
        draw_model(obj, shader);

        //light;
        light.use();
        view = camera.GetViewMatrix();
        light.setMat4("view", view);
        projection = perspective(radians(75.0f), (float)s_width / (float)s_height, 0.1f, 100.0f);
        light.setMat4("projection", projection);
        model = mat4(1.0f);
        lightPos = vec3(1.0f, 15.0f, 10.0f);
        model = translate(model, lightPos);
        light.setMat4("model", model);
        draw_model(prism, shader);
        //glDrawArrays(GL_TRIANGLES, 0, 36);

        screen->draw_widgets();
        glfwSwapBuffers(window);


        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}


void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        camera.position -= camera.right *= cameraSpeed * deltaTime;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        camera.position += camera.right *= cameraSpeed * deltaTime;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        camera.position -= camera.front *= cameraSpeed * deltaTime;
    }
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        camera.position += camera.front *= cameraSpeed * deltaTime;
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
    {
        camera.position += normalize(camera.worldUp) *= cameraSpeed * deltaTime;
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
    {
        camera.position -= normalize(camera.worldUp) *= cameraSpeed * deltaTime;
    }
    if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
    {
        angle -= 10.0f * deltaTime;
    }
    if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS)
    {
        angle += 10.0f * deltaTime;
    }
}

static void windowSizeCallback(GLFWwindow* window, int width, int height)
{
    glfwSetWindowSize(window, width, height);
    s_width = width;
    s_height = height;
}

static void cursorPosCallback(GLFWwindow* window, double xpos, double ypos)
{
    screen->cursor_pos_callback_event(xpos, ypos);
    double cur_x = xpos;
    double cur_y = ypos;
    double xoffset = (xpos - prev_x) * cameraSensitivity * deltaTime;
    double yoffset = (prev_y - ypos) * cameraSensitivity * deltaTime;
    prev_x = cur_x;
    prev_y = cur_y;

    camera.yaw += xoffset;
    camera.pitch += yoffset;
}
