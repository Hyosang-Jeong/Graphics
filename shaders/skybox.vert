/* Start Header -------------------------------------------------------
Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: skybox.vert
Purpose: vertex shader for skybox
Language: glsl
Platform: Microsoft Visual Studio2019, Windows
Project:  Hyosang Jung_CS300_1
Author: Hyosang Jung, hyosang.jung, 055587
Creation date: 2022 - 11 - 12
End Header --------------------------------------------------------*/
#version 450 core

layout (location = 0) in vec3 pos;
layout (location = 2) in vec2 uv;
uniform mat4 projection;

out vec2 TexCoords;
out vec3 aPos;

void main(void) 
{
    aPos = pos;
    TexCoords = uv;
    gl_Position = projection * vec4(pos, 1.0);
}

