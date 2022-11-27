/* Start Header -------------------------------------------------------
Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: FBO.h
Purpose: Manage Frame Buffer Object
Language: c++ Microsoft Visual Studio
Platform: Microsoft Visual Studio2019, Windows
Project:  Hyosang Jung_CS300_3
Author: Hyosang Jung, hyosang.jung, 055587
Creation date: 2022 - 11 - 10
End Header --------------------------------------------------------*/
#ifndef FBO_H
#define FBO_H
#include <GL/glew.h> 


class FBO
{
public:
	FBO();
	~FBO();
	void GenerateFBO();
	void Use();
	void UnUse();
	void AttachTexture(unsigned int texture);
private:
	GLuint fbo;
};
#endif