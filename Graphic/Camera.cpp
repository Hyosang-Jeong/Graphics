/* Start Header -------------------------------------------------------
Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: Camera.cpp
Purpose: Camera class for each scene
Language: c++ Microsoft Visual Studio
Platform: Microsoft Visual Studio2019, Windows
Project:  Hyosang Jung_CS300_1
Author: Hyosang Jung, hyosang.jung, 055587
Creation date: 2022 - 09 - 15
End Header --------------------------------------------------------*/
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
	Right = glm::normalize(glm::cross(Front, WorldUp));  
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

