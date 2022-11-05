/* Start Header -------------------------------------------------------
Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: LightUBO.h
Purpose: class for light uniform block managing
Language: c++ Microsoft Visual Studio
Platform: Microsoft Visual Studio2019, Windows
Project:  Hyosang Jung_CS300_2
Author: Hyosang Jung, hyosang.jung, 055587
Creation date: 2022 - 10 - 28
End Header --------------------------------------------------------*/
#ifndef LIGHTUBO_H
#define LIGHTUBO_H
#include <GL/glew.h> 
#include<vector>
#include"Light.h"
class LightUBO
{
public:
	void SetUniformBlock(GLint size, LightProperties* lights);
	void UpdateUniformBlock();
	void Use();

	GLuint uboHandle; // uniform buffer handle
	GLint blockSize;
	LightProperties* lights;
};
#endif