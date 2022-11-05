/* Start Header -------------------------------------------------------
Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: blinn.vert
Purpose: vertex shader for 3d object including light calculation using half vector
Language: glsl
Platform: Microsoft Visual Studio2019, Windows
Project:  Hyosang Jung_CS300_1
Author: Hyosang Jung, hyosang.jung, 055587
Creation date: 2022 - 09 - 12
End Header --------------------------------------------------------*/
#version 450 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 nrm;
layout (location = 2) in vec2 aTexCoords;

layout (location=1) out vec3 NRM;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 FragPos; 
out vec2 TexCoords;

void main(void) 
{
    gl_Position = projection * view * model * vec4(pos, 1.0);
    FragPos = vec3(model * vec4(pos, 1.0));
    NRM = mat3(transpose(inverse(model))) * nrm;
    TexCoords =  aTexCoords;
}

