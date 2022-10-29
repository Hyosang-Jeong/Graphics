/* Start Header -------------------------------------------------------
Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: Object.cpp
Purpose: Implement Object class functions.
Language: c++ Microsoft Visual Studio
Platform: Microsoft Visual Studio2019, Windows
Project:  Hyosang Jung_CS300_1
Author: Hyosang Jung, hyosang.jung, 055587
Creation date: 2022 - 09 - 14
End Header --------------------------------------------------------*/
#include"Object.h"
#include <glm/gtc/matrix_transform.hpp> //translate, rotate, scale, perspective 

Object::Object(GLuint shader)
{
    Shader_handle = shader;
}

void Object::Draw( glm::mat4 view, glm::mat4 projection)
{
    model->Use();
    GLint model_loc = glGetUniformLocation(Shader_handle, "model");
    GLint view_loc = glGetUniformLocation(Shader_handle, "view");
    GLint projection_loc = glGetUniformLocation(Shader_handle, "projection");
    glm::mat4 model_mat(1.0);
    model_mat = glm::translate(model_mat, position);
    model_mat = glm::rotate(model_mat, rotation.x, { 1,0,0 });
    model_mat = glm::rotate(model_mat, rotation.y, { 0,1,0 });
    model_mat = glm::rotate(model_mat, rotation.z, { 0,0,1 });
    model_mat = glm::scale(model_mat, { scale.x,scale.y,scale.z });

    glUniformMatrix4fv(model_loc, 1, GL_FALSE, &model_mat[0].x);
    glUniformMatrix4fv(view_loc, 1, GL_FALSE, &view[0].x);
    glUniformMatrix4fv(projection_loc, 1, GL_FALSE, &projection[0].x);
    glDrawElements(GL_TRIANGLES, model->numIndices, GL_UNSIGNED_INT, nullptr);
}

void Object::Draw_vtx_normal(glm::mat4 view, glm::mat4 projection)
{
    glm::mat4 model_mat(1.0);
    model_mat = glm::translate(model_mat, position);
    model_mat = glm::rotate(model_mat, rotation.x, { 1,0,0 });
    model_mat = glm::rotate(model_mat, rotation.y, { 0,1,0 });
    model_mat = glm::rotate(model_mat, rotation.z, { 0,0,1 });
    model_mat = glm::scale(model_mat, { scale.x,scale.y,scale.z });
    model->vertex_normal.Draw(view, projection, model_mat);
}
void Object::Draw_face_normal(glm::mat4 view, glm::mat4 projection)
{
    glm::mat4 model_mat(1.0);
    model_mat = glm::translate(model_mat, position);
    model_mat = glm::rotate(model_mat, rotation.x, { 1,0,0 });
    model_mat = glm::rotate(model_mat, rotation.y, { 0,1,0 });
    model_mat = glm::rotate(model_mat, rotation.z, { 0,0,1 });
    model_mat = glm::scale(model_mat, { scale.x,scale.y,scale.z });
    model->face_normal.Draw(view, projection, model_mat);
}