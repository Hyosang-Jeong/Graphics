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

layout(location=1) in vec3 NRM;

out vec4 outColor;
in vec3 FragPos; 

vec3 lightColor = vec3(1,1,1);

//ambient calculation
const float ambientStrength = 0.1f;
vec3 ambient = ambientStrength*lightColor;

uniform vec3 lightPos;
vec3 norm = normalize(NRM);
vec3 lightDir = normalize(lightPos - FragPos);  
float diff = max(dot(norm, lightDir), 0.0);
vec3 diffuse = diff * lightColor;

void main(void)
{
		vec3 result = (ambient + diffuse) * vec3(1,1,1);
		outColor =  vec4(result,1);
}