/* Start Header -------------------------------------------------------
Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: Engine.h
Purpose: Engine for running this program
Language: c++ Microsoft Visual Studio
Platform: Microsoft Visual Studio2019, Windows
Project:  Hyosang Jung_CS300_1
Author: Hyosang Jung, hyosang.jung, 055587
Creation date: 2022 - 09 - 10
End Header --------------------------------------------------------*/
#pragma once
#include<vector>
#include <GL/glew.h> 
#include <GLFW/glfw3.h>
#include"Scene/Scene.h"
class Engine
{
public:
	Engine(GLint w, GLint h);
	~Engine();
	void Add(Scene* scene);
	void init();
	void Update();
	void Draw();
	bool ShouldClose();
	void demo_switch();
private:
	std::vector<Scene*> scenes;
	GLFWwindow* ptr_window;
	int index;
};