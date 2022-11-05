/* Start Header -------------------------------------------------------
Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: Light.cpp
Purpose: class for one light
Language: c++ Microsoft Visual Studio
Platform: Microsoft Visual Studio2019, Windows
Project:  Hyosang Jung_CS300_2
Author: Hyosang Jung, hyosang.jung, 055587
Creation date: 2022 - 10 - 28
End Header --------------------------------------------------------*/
#include "Light.h"

Light::Light(LIGHT_TYPE type, Material material, glm::vec3 pos, glm::vec3 spot):
	type(type),material(material),position(pos),Inner_Outer_Falloff(spot)
{
}

void Light::UpdatePosition(glm::vec3 pos)
{
    position = pos;
}

void Light::UpdateAmbient(glm::vec3 ambient)
{
    material.ambient = ambient;
}

void Light::UpdateDiffuse(glm::vec3 diffuse)
{
    material.diffuse = diffuse;
}

void Light::UpdateSpecular(glm::vec3 specular)
{
    material.specular = specular;
}
