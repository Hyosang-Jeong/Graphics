/* Start Header -------------------------------------------------------
Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: Object.h
Purpose: Declare Object class.
Language: c++ Microsoft Visual Studio
Platform: Microsoft Visual Studio2019, Windows
Project:  Hyosang Jung_CS300_1
Author: Hyosang Jung, hyosang.jung, 055587
Creation date: 2022 - 09 - 14
End Header --------------------------------------------------------*/
#pragma once
#include"Model.h"
#include<glm/glm.hpp>

class Object
{
public:
    Object() = default;
    Object(GLuint shader);

    void Draw_vtx_normal( glm::mat4 view, glm::mat4 projection);
    void Draw_face_normal( glm::mat4 view, glm::mat4 projection);
    void Draw( glm::mat4 view, glm::mat4 projection);
    GLuint Shader_handle;
    Model* model;

    glm::vec3 position{ 0,0,0 };
    glm::vec3 scale = { 1,1,1 };
    glm::vec3 rotation{ 0,0,0 };
};