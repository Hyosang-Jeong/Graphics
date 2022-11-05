/* Start Header -------------------------------------------------------
Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: LightUBO.cpp
Purpose: class for light uniform block managing
Language: c++ Microsoft Visual Studio
Platform: Microsoft Visual Studio2019, Windows
Project:  Hyosang Jung_CS300_2
Author: Hyosang Jung, hyosang.jung, 055587
Creation date: 2022 - 10 - 28
End Header --------------------------------------------------------*/
#include"LightUBO.h"


void LightUBO::SetUniformBlock(GLint size, LightProperties* Lights)
{
    blockSize = size;
    lights = Lights;
    glGenBuffers(1, &uboHandle);
    glBindBuffer(GL_UNIFORM_BUFFER, uboHandle);
    glBufferData(GL_UNIFORM_BUFFER, blockSize, NULL, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    glBindBufferRange(GL_UNIFORM_BUFFER, 0, uboHandle, 0, blockSize);
   
}

void LightUBO::UpdateUniformBlock()
{
    //Light size = 96
    glBindBuffer(GL_UNIFORM_BUFFER, uboHandle);

    for (int i = 0; i < 16; i++)
    {
        int type = static_cast<int>(lights->light[i].type);
         glBufferSubData(GL_UNIFORM_BUFFER,(i * 112),           sizeof(int), &type);
         glBufferSubData(GL_UNIFORM_BUFFER,(i * 112)+ 16,    sizeof(glm::vec3), &lights->light[i].material.ambient);
         glBufferSubData(GL_UNIFORM_BUFFER,(i * 112)+ 32,    sizeof(glm::vec3), &lights->light[i].material.diffuse);
         glBufferSubData(GL_UNIFORM_BUFFER,(i * 112)+48,     sizeof(glm::vec3),  &lights->light[i].material.specular);
         glBufferSubData(GL_UNIFORM_BUFFER,(i * 112)+64,     sizeof(glm::vec3),  &lights->light[i].position);
         glBufferSubData(GL_UNIFORM_BUFFER,(i * 112)+80,     sizeof(glm::vec3),  &lights->light[i].direction);
         glBufferSubData(GL_UNIFORM_BUFFER, (i * 112) + 96, sizeof(glm::vec3), &lights->light[i].Inner_Outer_Falloff);
    }

    glBufferSubData(GL_UNIFORM_BUFFER, 112 * 16,            sizeof(glm::vec3), &lights->coefficients);
    glBufferSubData(GL_UNIFORM_BUFFER, (112 * 16) + 16, sizeof(glm::vec3), &lights->fogColor);
    glBufferSubData(GL_UNIFORM_BUFFER, (112 * 16) + 32, sizeof(glm::vec3), &lights->GlobalAmbient);
    glBufferSubData(GL_UNIFORM_BUFFER, (112 * 16) + 44,  sizeof(int), &lights->activeLight);
    glBufferSubData(GL_UNIFORM_BUFFER, (112 * 16) + 48, sizeof(int), &lights->fog_far);
    glBufferSubData(GL_UNIFORM_BUFFER, (112 * 16) + 52, sizeof(int), &lights->fog_near);

    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void LightUBO::Use()
{
    glBindBuffer(GL_UNIFORM_BUFFER, uboHandle);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, uboHandle);
}
