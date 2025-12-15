/*
* Temporary file used for debugging model loader.
*/

#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace glm;
using namespace std;
#include <string>
#include <iostream>
#include <filesystem>

#include "shader.h"
#include "camera.h"
#include "model.h"

GLFWwindow* window = NULL;
float cameraSensitivity = 150.0f;
float cameraSpeed = 10.0f;
float deltaTime = 0.0f;
float lastFrame = 0.0f;
double prev_x = 0.0;
double prev_y = 0.0;
int s_width = 1280;
int s_height = 720;
float angle = 0.0f;

//prototypes
void processInput(GLFWwindow* window);
static void windowSizeCallback(GLFWwindow* window, int width, int height);
static void cursorPosCallback(GLFWwindow* window, double xpos, double ypos);

Camera camera(vec3(0.0, 2.0f, 3.0f), vec3(0.0, 1.0f, 0.0f));
int main()
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

        if (!gladLoadGL(glfwGetProcAddress))
	{
		cout << "Failed to initialize GLAD" << endl;
		return -1;
	}

	//set callbacks
	glfwSetCursorPosCallback(window, cursorPosCallback);
        glfwSetWindowSizeCallback(window, windowSizeCallback);
	
	glViewport(0, 0, s_width, s_height);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        cout << "Path: " << filesystem::current_path() << endl;

	Shader shader = Shader("shaders/cube.vert", "shaders/cube.frag");
	shader.use();

        Shader light = Shader("shaders/cube.vert", "shaders/light.frag");

	vec3 cameraPos = vec3(0.0f, 0.0f, 2.0f);
	vec3 lightPos = vec3(1.0f, 1.5f, 2.0f);

	glEnable(GL_DEPTH_TEST);

	Model obj;
	obj = load_model(load_obj("models/t_34/t_34_obj.obj"));

	cout << "Read " << obj.vertices << " vertices, " << obj.indices << " indices." << endl;

	Model obj2 = load_model(load_obj("models/zis-42/ZIS_42.obj"));

        Model prism = load_model(load_obj("models/prism.obj"));

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	prev_x = s_width / 2.0f;
	prev_y = s_height / 2.0f;

	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		processInput(window);

		glViewport(0, 0, s_width, s_height);
		glClearColor(0.35f, 0.73f, 0.83f, 1.0f);
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
		model = scale(model, vec3(0.1f));
		shader.setMat4("model", model);
		shader.setMat4("view", view);
		shader.setMat4("projection", projection);
                shader.setVec3("objectColor", 0.5, 0.5, 0.9);
		draw_model(obj);
		int i = glGetError();

		//draw other thingy
		model = mat4(1.0f);
                model = scale(model, vec3(0.01f));
		model = translate(model, vec3(5.0f, 2.0f, -3.0f));
                model = rotate(model, radians(-90.0f), vec3(1.0f, 0.0f, 0.0f));
		shader.setMat4("model", model);
		draw_model(obj2);

		//light;
		light.use();
		view = camera.GetViewMatrix();
		light.setMat4("view", view);
                projection = perspective(radians(75.0f), (float)s_width / (float)s_height, 0.1f, 100.0f);
		light.setMat4("projection", projection);
		model = mat4(1.0f);
                lightPos = vec3(1.0f, 3.0f, 1.0f);
		model = translate(model, lightPos);
		light.setMat4("model", model);
		draw_model(prism);
		//glDrawArrays(GL_TRIANGLES, 0, 36);
		
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
	double cur_x = xpos;
	double cur_y = ypos;
	double xoffset = (xpos - prev_x) * cameraSensitivity * deltaTime;
	double yoffset = (prev_y - ypos) * cameraSensitivity * deltaTime;
	prev_x = cur_x;
	prev_y = cur_y;

	camera.yaw += xoffset;
	camera.pitch += yoffset;
}
