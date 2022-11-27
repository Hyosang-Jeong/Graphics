/* Start Header -------------------------------------------------------
Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: FBO.cpp
Purpose: Manage Frame Buffer Object
Language: c++ Microsoft Visual Studio
Platform: Microsoft Visual Studio2019, Windows
Project:  Hyosang Jung_CS300_3
Author: Hyosang Jung, hyosang.jung, 055587
Creation date: 2022 - 11 - 10
End Header --------------------------------------------------------*/
#include "FBO.h"

FBO::FBO()
{
	GenerateFBO();
}

FBO::~FBO()
{
	glDeleteFramebuffers(1, &fbo);
}

void FBO::GenerateFBO()
{
	glGenFramebuffers(1, &fbo); // create the FB object handle
}

void FBO::Use()
{
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
}
void FBO::UnUse()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
void FBO::AttachTexture(unsigned int texture)
{
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
}
