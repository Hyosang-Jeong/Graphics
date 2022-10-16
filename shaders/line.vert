/* Start Header -------------------------------------------------------
Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: line.vert
Purpose: vertex shader for line drawing
Language: glsl
Platform: Microsoft Visual Studio2019, Windows
Project:  Hyosang Jung_CS300_1
Author: Hyosang Jung, hyosang.jung, 055587
Creation date: 2022 - 09 - 12
End Header --------------------------------------------------------*/
#version 450 core

layout (location = 0) in vec3 pos;

uniform mat4 view;
uniform mat4 projection;
uniform mat4 model;

void main(void) 
{
        gl_Position = projection * view  *model* vec4(pos, 1.0);
}

