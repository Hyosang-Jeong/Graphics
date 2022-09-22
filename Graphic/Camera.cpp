#include "Camera.h"


Camera::Camera()
{
	SetPosition({ 0,0,10 });
}

Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch)
{
	Position = position;
	WorldUp = up;
	Yaw = yaw;
	Pitch = pitch;
}

glm::mat4 Camera::GetCamera()
{
	glm::vec3 front;
	front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
	front.y = sin(glm::radians(Pitch));
	front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
	Front = glm::normalize(front);
	// also re-calculate the Right and Up vector
	Right = glm::normalize(glm::cross(Front, WorldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
	Up = glm::normalize(glm::cross(Right, Front));
	return glm::lookAt(Position, Position + Front, Up);;
}

void Camera::SetPosition(glm::vec3 position)
{
	Position = position;
}

void Camera::SetYaw(float yaw)
{
	Yaw = yaw;
}

void Camera::SetPitch(float pitch)
{
	Pitch = pitch;
}

