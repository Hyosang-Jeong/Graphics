/* Start Header -------------------------------------------------------
Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: simple_render.frag
Purpose: fragment shader for 3d object including light calculation
Language: glsl
Platform: Microsoft Visual Studio2019, Windows
Project:  Hyosang Jung_CS300_1
Author: Hyosang Jung, hyosang.jung, 055587
Creation date: 2022 - 09 - 12
End Header --------------------------------------------------------*/
#version 450 core

in vec3 outColor; 
out vec4 Color;

void main(void)
{
	Color =  vec4(outColor,1);
}