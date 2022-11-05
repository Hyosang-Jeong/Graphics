/* Start Header -------------------------------------------------------
Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: Light.h
Purpose: class for one light
Language: c++ Microsoft Visual Studio
Platform: Microsoft Visual Studio2019, Windows
Project:  Hyosang Jung_CS300_2
Author: Hyosang Jung, hyosang.jung, 055587
Creation date: 2022 - 10 - 28
End Header --------------------------------------------------------*/
#ifndef LIGHT_H
#define LIGHT_H
#include<vector>
#include <GL/glew.h> 
#include<glm/glm.hpp>
enum class LIGHT_TYPE
{
	POINT,
	DIRECTIONAL,
	SPOT
};
using namespace glm;

struct Material
{
	glm::vec3 ambient{ 1 };
	glm::vec3 diffuse{ 1 };
	glm::vec3 specular{ 1 };
	glm::vec3 emissive{ 1 };
};

class Light
{
public:
	Light() = default;
	Light(LIGHT_TYPE type, Material material, glm::vec3 pos, glm::vec3 spot);

	void UpdatePosition(glm::vec3 pos);
	void UpdateAmbient(glm::vec3 ambient);
	void UpdateDiffuse(glm::vec3 diffuse);
	void UpdateSpecular(glm::vec3 specular);

	LIGHT_TYPE type;
	Material material;
	glm::vec3 position;
	glm::vec3 direction;
	glm::vec3 Inner_Outer_Falloff;
};

class LightProperties
{
public:
	Light light[16];
	glm::vec3 coefficients{2.f,0.2f,0.2f};
	glm::vec3 fogColor{1.f,1.f,1.f};
	glm::vec3 GlobalAmbient{0.f,0.f,1.f};
	int activeLight{ 1 };
	float fog_far{50.f};
	float fog_near{10.f};
};

#endif 

