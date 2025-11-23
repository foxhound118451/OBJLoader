/*
* Temporary file used for debugging model loader.
*/

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace glm;
using namespace std;
#include <string>
#include <iostream>

#include <shader.h>
#include <camera.h>
#include <model.h>

void processInput(GLFWwindow* window);
void windowSizeCallback(GLFWwindow* window, int width, int height);
float cameraSensitivity = 90.0f;
float cameraSpeed = 10.0f;
float deltaTime = 0.0f;
float lastFrame = 0.0f;
int s_width = 1280;
int s_height = 720;

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
	GLFWwindow* window = glfwCreateWindow(s_width, s_height, "OpenGL", NULL, NULL);

	if (!window)
	{
		cout << "Failed to create GLFW window" << endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		cout << "Failed to initialize GLAD" << endl;
		return -1;
	}

	//set callbacks
	glfwSetWindowSizeCallback(window, windowSizeCallback);
	
	glViewport(0, 0, s_width, s_height);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	Shader shader = Shader("src/cube.vert", "src/cube.frag");
	shader.use();

	vec3 cameraPos = vec3(0.0f, 0.0f, 2.0f);
	vec3 lightPos = vec3(3.0f, 2.5f, 4.0f);

	glEnable(GL_DEPTH_TEST);

	Mesh mesh;
	mesh = loadObjFromPath("models/Untitled.obj");

	Mesh cube;
	cube = loadObjFromPath("models/Cube.obj");


	int len = mesh.vertices.size();
	for (int i = 0; i < len; ++i)
	{
		Vertex vertex = mesh.vertices.at(i);
		cout << "v" << i + 1 << ": " << vertex.x << ", " << vertex.y << ", " << vertex.z << endl;
		cout << "vt" << i + 1 << ": " << vertex.tx << ", " << vertex.ty << endl;
		cout << "vn" << i + 1 << ": " << vertex.nx << ", " << vertex.ny << ", " << vertex.nz << endl;
	}

	len = mesh.indices.size();
	for (int i = 0; i < len; ++i)
	{
		cout << "i" << i + 1 << ": " << mesh.indices[i] << endl;
	}


	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		processInput(window);

		glViewport(0, 0, s_width, s_height);
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
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

		//draw cube model
		mat4 model = mat4(1.0f);
		model = translate(model, vec3(0.0f, 2.0f, -3.0f));
		shader.setMat4("model", model);
		shader.setMat4("view", view);
		shader.setMat4("projection", projection);
		drawMesh(mesh);

		//light;
		shader.use();
		model = mat4(1.0f);
		model = translate(model, lightPos);
		shader.setMat4("model", model);
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
		camera.yaw -= cameraSensitivity * deltaTime;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		camera.yaw += cameraSensitivity * deltaTime;
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		camera.pitch -= cameraSensitivity * deltaTime;
	}
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		camera.pitch += cameraSensitivity *  deltaTime;
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
	{
		camera.position -= camera.right *= cameraSpeed * deltaTime;
	}
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
	{
		camera.position += camera.right *= cameraSpeed * deltaTime;
	}
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
	{
		camera.position -= camera.front *= cameraSpeed * deltaTime;
	}
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
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
}

void windowSizeCallback(GLFWwindow* window, int width, int height)
{
	glfwSetWindowSize(window, width, height);
	s_width = width;
	s_height = height;
}