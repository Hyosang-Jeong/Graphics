/* Start Header -------------------------------------------------------
Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: simple_render.vert
Purpose: vertex shader for 3d object
Language: glsl
Platform: Microsoft Visual Studio2019, Windows
Project:  Hyosang Jung_CS300_1
Author: Hyosang Jung, hyosang.jung, 055587
Creation date: 2022 - 09 - 12
End Header --------------------------------------------------------*/
#version 450 core

layout (location = 0) in vec3 pos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main(void) 
{
    mat4 MVP =  projection * view * model;
    gl_Position =  MVP * vec4(pos, 1.0);
}

