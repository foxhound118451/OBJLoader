#ifndef CAMERA_H
#define CAMERA_H
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>
using namespace glm;

class Camera
{
public:
	vec3 position;
	vec3 worldUp;
	vec3 front;
	vec3 up;
	vec3 right;
	float pitch;
	float yaw;

	Camera(vec3 position, vec3 worldUp, float pitch = 0.0f, float yaw = -90.0f)
	{
		this->position = position;
		this->worldUp = worldUp;
		this->pitch = pitch;
		this->yaw = yaw;
		updateCameraVectors();
	}

	mat4 GetViewMatrix()
	{
		return LookAt(position, position + front, worldUp);
	}

	/*
	* Update camera orientation based on pitch and yaw 
	*/
	void updateCameraVectors()
	{
		pitch = glm::min(89.0f, pitch);
		pitch = glm::max(-89.0f, pitch);

		vec3 direction;
		direction.x = cos(radians(yaw)) * cos(radians(pitch));
		direction.y = sin(radians(pitch));
		direction.z = sin(radians(yaw)) * cos(radians(pitch));

		front = normalize(direction);
		right = normalize(cross(front, worldUp));
		up = normalize(cross(right, front));
	}
private:

	/*Returns matrix which transforms vector to inverse of camera position and rotation*/
	mat4 LookAt(vec3 pos, vec3 target, vec3 up)
	{
		mat4 translation = mat4(1.0f);
		translation[3][0] = -pos.x;
		translation[3][1] = -pos.y;
		translation[3][2] = -pos.z;

		vec3 zaxis = normalize(pos - target);
		vec3 xaxis = normalize(cross(normalize(up), zaxis));
		vec3 yaxis = cross(zaxis, xaxis);

		mat4 rotation = mat4(1.0f); //inverse of camera rotation
		rotation[0][0] = xaxis.x;
		rotation[1][0] = xaxis.y;
		rotation[2][0] = xaxis.z;
		rotation[0][1] = yaxis.x;
		rotation[1][1] = yaxis.y;
		rotation[2][1] = yaxis.z;
		rotation[0][2] = zaxis.x;
		rotation[1][2] = zaxis.y;
		rotation[2][2] = zaxis.z;

		return rotation * translation;
	}
};
#endif
