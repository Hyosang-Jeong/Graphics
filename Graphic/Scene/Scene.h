/* Start Header -------------------------------------------------------
Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: Scene.h
Purpose: Base class for the scene. Each scene has inherited this class
Language: c++ Microsoft Visual Studio
Platform: Microsoft Visual Studio2019, Windows
Project:  Hyosang Jung_CS300_1
Author: Hyosang Jung, hyosang.jung, 055587
Creation date: 2022 - 09 - 10
End Header --------------------------------------------------------*/
#pragma once
#include <glm/gtc/matrix_transform.hpp>
class Scene
{
public:
	Scene() = default;
	virtual ~Scene() {};

	virtual void init() = 0;
	virtual void Update(float) = 0;
	virtual void Draw() = 0;
	virtual void OnImGuiRender() = 0;
	virtual void UnLoad() = 0;

protected:
    glm::mat4 projection =  glm::perspective(glm::radians(90.0f), 1.f, 0.1f, 100.0f);
};