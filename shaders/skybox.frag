/* Start Header -------------------------------------------------------
Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: skybox.frag
Purpose: fragment shader for skybox
Language: glsl
Platform: Microsoft Visual Studio2019, Windows
Project:  Hyosang Jung_CS300_1
Author: Hyosang Jung, hyosang.jung, 055587
Creation date: 2022 - 11 - 12
End Header --------------------------------------------------------*/
#version 450 core

out vec4 Color;
in vec2 TexCoords;
in vec3 aPos;

uniform sampler2D skybox_LEFT;
uniform sampler2D skybox_RIGHT;
uniform sampler2D skybox_BOTTOM;
uniform sampler2D skybox_TOP;
uniform sampler2D skybox_BACK;
uniform sampler2D skybox_FRONT;

vec4 GenerateCubeColor()
{
	vec3 absVec = abs(aPos);
	vec3 v = aPos;
	vec2 uv;
	if( absVec.x >= absVec.y && absVec.x >= absVec.z )	
	{
            (v.x < 0.0) ? (uv.x =-v.z/absVec.x) : (uv.x = v.z / absVec.x);
            uv.y = v.y;
			uv = (uv+1.f)/2.f;
		if(aPos.x < 0)
			return texture(skybox_RIGHT, uv);
		else
			return texture(skybox_LEFT, uv);
	}
	else if( absVec.y >= absVec.x && absVec.y >= absVec.z )	
	{
            (v.y < 0.0) ? (uv.y = v.z / absVec.y) : (uv.y =  -v.z / absVec.y);
            uv.x =-v.x / absVec.y;
			uv = (uv+1.f)/2.f;
		if(aPos.y < 0)
				return texture(skybox_BOTTOM, uv);
		else
				return texture(skybox_TOP, uv);
	}
	else if( absVec.z >= absVec.x && absVec.z >= absVec.y )	
	{
	     (v.z < 0.0) ? (uv.x = v.x / absVec.z) : (uv.x = -v.x / absVec.z);
         uv.y = v.y / absVec.z;
		 uv = (uv+1.f)/2.f;
		if(aPos.z < 0)
			return texture(skybox_BACK, uv);
		else
			return texture(skybox_FRONT, uv);
	}
}

void main(void)
{
	 Color =  GenerateCubeColor();
}