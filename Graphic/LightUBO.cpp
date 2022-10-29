#include"LightUBO.h"


void LightUBO::SetUniformBlock(GLint size, LightProperties* Lights)
{
    blockSize = size;
    lights = Lights;
    glGenBuffers(1, &uboHandle);
    glBindBuffer(GL_UNIFORM_BUFFER, uboHandle);
    glBufferData(GL_UNIFORM_BUFFER, blockSize, NULL, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    glBindBufferRange(GL_UNIFORM_BUFFER, 0, uboHandle, 0, blockSize);
}

void LightUBO::UpdateUniformBlock()
{
    //Light size = 96
    glBindBuffer(GL_UNIFORM_BUFFER, uboHandle);

    for (int i = 0; i < 16; i++)
    {
        int type = static_cast<int>(lights->light[0].type);
         glBufferSubData(GL_UNIFORM_BUFFER,(i * 96 ), sizeof(int), &type);
         glBufferSubData(GL_UNIFORM_BUFFER,(i * 96 )+ 16, sizeof(glm::vec3), &lights->light[0].material.ambient);
         glBufferSubData(GL_UNIFORM_BUFFER,(i * 96 )+ 32, sizeof(glm::vec3), &lights->light[0].material.diffuse);
         glBufferSubData(GL_UNIFORM_BUFFER,(i * 96 )+48, sizeof(glm::vec3), &lights->light[0].material.specular);
         glBufferSubData(GL_UNIFORM_BUFFER,(i * 96 )+64, sizeof(glm::vec3), &lights->light[0].position);
         glBufferSubData(GL_UNIFORM_BUFFER,(i * 96 )+80, sizeof(glm::vec3), &lights->light[0].direction);
    }

    glBufferSubData(GL_UNIFORM_BUFFER, 96 *16, sizeof(int), &lights->activeLight);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void LightUBO::Use()
{
    glBindBuffer(GL_UNIFORM_BUFFER, uboHandle);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, uboHandle);
}
