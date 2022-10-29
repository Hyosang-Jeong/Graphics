#include "Light.h"

Light::Light(LIGHT_TYPE type, Material material, glm::vec3 pos):
	type(type),material(material),position(pos)
{
}

//void Light::SetUniformBlock(GLuint Shader_handle)
//{
//    GLuint blockIndex = glGetUniformBlockIndex(Shader_handle, "LightProperties");
//
//    glGetActiveUniformBlockiv(Shader_handle, blockIndex, GL_UNIFORM_BLOCK_DATA_SIZE, &blockSize);
//    int a = sizeof(LightProperties);
//    lightMaterialBuffer = new GLubyte[blockSize];
//
//    constexpr int num = 2;
//    const GLchar* names[num] = { "LightProperties.activeLight", ".light[0]"};
//    GLuint indices[num];
//    glGetUniformIndices(Shader_handle, num, names, indices);
//    offset.resize(num);
//    glGetActiveUniformsiv(Shader_handle, num, indices, GL_UNIFORM_OFFSET, &offset[0]);
//
//    memcpy(lightMaterialBuffer + offset[0], &position, sizeof(position));
//    memcpy(lightMaterialBuffer + offset[1], &material.ambient, sizeof(material.ambient));
//    memcpy(lightMaterialBuffer + offset[2], &material.diffuse, sizeof(material.diffuse));
//    memcpy(lightMaterialBuffer + offset[3], &material.specular, sizeof(material.specular));
//
//    glGenBuffers(1, &uboHandle);
//    glBindBuffer(GL_UNIFORM_BUFFER, uboHandle);
//    glBufferData(GL_UNIFORM_BUFFER, blockSize, lightMaterialBuffer, GL_DYNAMIC_DRAW);
//}

//void Light::UpdateUniformBlock()
//{
//    memcpy(lightMaterialBuffer + offset[0], &position, sizeof(position));
//    memcpy(lightMaterialBuffer + offset[1], &material.ambient, sizeof(material.ambient));
//    memcpy(lightMaterialBuffer + offset[2], &material.diffuse, sizeof(material.diffuse));
//    memcpy(lightMaterialBuffer + offset[3], &material.specular, sizeof(material.specular));
//    glBindBuffer(GL_UNIFORM_BUFFER, uboHandle);
//    glBufferData(GL_UNIFORM_BUFFER, blockSize, lightMaterialBuffer, GL_DYNAMIC_DRAW);
//}
//
//void Light::Use()
//{
//    glBindBuffer(GL_UNIFORM_BUFFER, uboHandle);
//    glBindBufferBase(GL_UNIFORM_BUFFER, 0, uboHandle);
//}

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
