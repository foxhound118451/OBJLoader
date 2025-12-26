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
float rotation_speed = 0.0f;
float FOV = 75.0f;

glm::vec3 model_pos(0.0f);

//prototypes
void processInput(GLFWwindow* window);
static void windowSizeCallback(GLFWwindow* window, int width, int height);
static void cursorPosCallback(GLFWwindow* window, double xpos, double ypos);

Camera camera(vec3(0.0, 0.5f, 2.25f), vec3(0.0, 1.0f, 0.0f));

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

    


    glViewport(0, 0, s_width, s_height);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    cout << "Path: " << filesystem::current_path() << endl;

    Shader shader = Shader("shaders/model.vert", "shaders/model.frag");
    shader.use();

    Shader light = Shader("shaders/model.vert", "shaders/light.frag");

    vec3 cameraPos = vec3(0.0f, 0.0f, 0.0f);
    vec3 lightPos = vec3(0.0f, 1.5f, 1.0f);

    Model obj;
    if (argc == 2)
    {
        obj = load_model(load_obj(argv[1]));
    }
    else
    {
        std::cout << "Usage: OBJViewer [path]." << std::endl;
        return 1;
    }

    cout << "Read " << obj.vertice_count << " vertices, " << obj.indice_count << " indices." << endl;


    Model prism = load_model(load_obj("models/prism.obj"));

    prev_x = s_width / 2.0f;
    prev_y = s_height / 2.0f;

    //init nanogui screen
    screen = new nanogui::Screen();
    screen->initialize(window, true);

    //create nanogui gui
    nanogui::FormHelper *gui = new nanogui::FormHelper(screen);
    nanogui::ref<nanogui::Window> transform_window = gui->add_window(nanogui::Vector2i(10,10), "Model transformation");

    gui->add_group("Position");

    glm::vec3 model_scale = glm::vec3(1/obj.scale_factor); //normalize model scale 
    float rotation_angle = 0.0f;

    gui->add_variable("X", model_pos.x, true)->set_spinnable(true);
    gui->add_variable("Y", model_pos.y, true)->set_spinnable(true);
    gui->add_variable("Z", model_pos.z, true)->set_spinnable(true);

    gui->add_group("Scale");
    gui->add_variable("X", model_scale.x, true)->set_spinnable(true);
    gui->add_variable("Y", model_scale.y, true)->set_spinnable(true);
    gui->add_variable("Z", model_scale.z, true)->set_spinnable(true);

    gui->add_group("Rotation");
    gui->add_variable("Angle (Radians)", rotation_angle, true)->set_spinnable(true);

    screen->set_visible(true);

    //layout
    screen->perform_layout();

    //set callbacks
    glfwSetCursorPosCallback(window, cursorPosCallback);
    glfwSetWindowSizeCallback(window, windowSizeCallback);
    glfwSetFramebufferSizeCallback(window, [](GLFWwindow *, int width, int height) {
            screen->resize_callback_event(width,height);
            }
    );
    glfwSetMouseButtonCallback(window, [](GLFWwindow *, int button, int action, int mods) {
            screen->mouse_button_callback_event(button, action, mods);
            }
    );
    glfwSetKeyCallback(window, [](GLFWwindow *, int key, int scancode, int action, int mods) {
            screen->key_callback_event(key, scancode, action, mods);
            }
    );
    glfwSetCharCallback(window, [](GLFWwindow *, unsigned int codepoint) {
            screen->keyboard_character_event(codepoint);
            }
    );

    while (!glfwWindowShouldClose(window))
    {
        glEnable(GL_DEPTH_TEST);

        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        glViewport(0, 0, s_width, s_height);
        glClearColor(0.7f, 0.7f, 0.7f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        camera.updateCameraVectors();

        shader.use();
        mat4 view = camera.GetViewMatrix();
        shader.setMat4("view", view);
        mat4 projection = perspective(radians(FOV), (float)s_width / (float)s_height, 0.1f, 100.0f);
        shader.setMat4("projection", projection);

        //lighting uniforms
        shader.setVec3("lightPos", lightPos);
        shader.setVec3("ambient", vec3(0.6f, 0.6f, 0.2f));

        rotation_speed = clamp(rotation_speed, -0.1f, 0.1f);
        if (rotation_angle > (2.0f * glm::pi<float>()) || rotation_angle < -(2.0f * glm::pi<float>()))
        {
            rotation_angle = 0.0f;
        }

        rotation_angle += rotation_speed;
        if (rotation_speed != 0)
        {
            rotation_speed -= (rotation_speed * 0.9f * deltaTime);
        }

        mat4 model = mat4(1.0f);
        model = translate(model, model_pos);
        model = scale(model, model_scale);
        model = rotate(model, rotation_angle, vec3(0.0f, 1.0f, 0.0f));
        shader.setMat4("model", model);
        shader.setMat4("view", view);
        shader.setMat4("projection", projection);
        shader.setVec3("objectColor", 0.5, 0.5, 0.9);
        shader.setVec3("ViewPos", camera.position);
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

        gui->refresh();
        screen->draw_widgets();

        glfwSwapBuffers(window);
        glfwPollEvents();
        processInput(window);
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
   // if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
   // {
   //     camera.position -= camera.right *= cameraSpeed * deltaTime;
   // }
   // if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
   // {
   //     camera.position += camera.right *= cameraSpeed * deltaTime;
   // }
   // if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
   // {
   //     camera.position -= camera.front *= cameraSpeed * deltaTime;
   // }
   // if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
   // {
   //     camera.position += camera.front *= cameraSpeed * deltaTime;
   // }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
    {
        model_pos.y += 2.5f * deltaTime;
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
    {
        model_pos.y -= 2.5f * deltaTime;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        rotation_speed -= 0.05f * deltaTime;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        rotation_speed += 0.05f * deltaTime;
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
    {
        model_pos.x -= 2.5f * deltaTime;
    }
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
    {
        model_pos.x += 2.5f * deltaTime;
    }
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
    {
        model_pos.z -= 2.5f * deltaTime;
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
    {
        model_pos.z += 2.5f * deltaTime;
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
   if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS)
   {
       rotation_speed += 0.02f * xoffset * deltaTime;
   }


   // camera.yaw += xoffset;
   // camera.pitch += yoffset;
}
