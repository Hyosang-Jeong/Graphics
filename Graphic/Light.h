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
};

class Light
{
public:
	Light() = default;
	Light(LIGHT_TYPE type, Material material, glm::vec3 pos);

	void UpdatePosition(glm::vec3 pos);
	void UpdateAmbient(glm::vec3 ambient);
	void UpdateDiffuse(glm::vec3 diffuse);
	void UpdateSpecular(glm::vec3 specular);

	LIGHT_TYPE type;
	Material material;
	glm::vec3 position;
	glm::vec3 direction;
};

class LightProperties
{
public:
	int activeLight;
	Light light[16];
};

#endif 