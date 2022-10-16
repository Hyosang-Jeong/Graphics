/* Start Header -------------------------------------------------------
Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: Camera.h
Purpose: Camera class for each scene
Language: c++ Microsoft Visual Studio
Platform: Microsoft Visual Studio2019, Windows
Project:  Hyosang Jung_CS300_1
Author: Hyosang Jung, hyosang.jung, 055587
Creation date: 2022 - 09 - 15
End Header --------------------------------------------------------*/
#include <glm/gtc/matrix_transform.hpp>


class Camera
{
public:
	Camera();
    Camera(glm::vec3 position, glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = -90.0f, float pitch = 0);
    glm::mat4 GetCamera();
	void SetPosition(glm::vec3 position);
    void SetYaw(float yaw);
    void SetPitch(float pitch);
	glm::vec3 GetPosition() { return Position; }
    float GetYaw() { return Yaw; }
    float GetPitch() { return Pitch; }
private:
    glm::vec3 Position{ 0,0,0 };
    glm::vec3 Front{ 0,0,-1.f };
    glm::vec3 Up{ 0,1.f,0 };
    glm::vec3 Right;
    glm::vec3 WorldUp;
    // euler Angles
    float Yaw{ -90.0f};
    float Pitch{ 0 };
};