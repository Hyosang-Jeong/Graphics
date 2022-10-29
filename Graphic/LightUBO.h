#ifndef LIGHTUBO_H
#define LIGHTUBO_H
#include <GL/glew.h> 
#include<vector>
#include"Light.h"
class LightUBO
{
public:
	void SetUniformBlock(GLint size, LightProperties* lights);
	void UpdateUniformBlock();
	void Use();

	GLuint uboHandle; // uniform buffer handle
	GLint blockSize;
	LightProperties* lights;
};
#endif