#include"Object.h"
#include <glm/gtc/matrix_transform.hpp> //translate, rotate, scale, perspective 

Object::Object(std::string shader)
{
    setup_shdrpgm(shader);
}



void Object::Draw( Model& vertex_array, glm::mat4 view, glm::mat4 projection)
{
    vertex_array.Use();
    GLint model_loc = glGetUniformLocation(Shader_handle, "model");
    GLint view_loc = glGetUniformLocation(Shader_handle, "view");
    GLint projection_loc = glGetUniformLocation(Shader_handle, "projection");
    glm::mat4 model(1.0);
    model = glm::translate(model, position);
    model = glm::rotate(model, rotation.x, { 1,0,0 });
    model = glm::rotate(model, rotation.y, { 0,1,0 });
    model = glm::rotate(model, rotation.z, { 0,0,1 });
    model = glm::scale(model, { scale.x,scale.y,scale.z });

    glUniformMatrix4fv(model_loc, 1, GL_FALSE, &model[0].x);
    glUniformMatrix4fv(view_loc, 1, GL_FALSE, &view[0].x);
    glUniformMatrix4fv(projection_loc, 1, GL_FALSE, &projection[0].x);
    glDrawElements(GL_TRIANGLES, vertex_array.numIndices, GL_UNSIGNED_INT, nullptr);
}

void Object::setup_shdrpgm(std::string shader)
{
    std::string vert = "../shaders/";
    std::string frag = "../shaders/";
    vert = vert + shader + ".vert";
    frag = frag + shader + ".frag";

    Shader_handle = LoadShaders(vert.c_str(), frag.c_str());
}

void Object::Draw_vtx_normal(Model& vertex_array, glm::mat4 view, glm::mat4 projection)
{
    glm::mat4 model(1.0);
    model = glm::translate(model, position);
    model = glm::rotate(model, rotation.x, { 1,0,0 });
    model = glm::rotate(model, rotation.y, { 0,1,0 });
    model = glm::rotate(model, rotation.z, { 0,0,1 });
    model = glm::scale(model, { scale.x,scale.y,scale.z });
    vertex_array.vertex_normal.Draw(view, projection, model);
}
void Object::Draw_face_normal(Model& vertex_array, glm::mat4 view, glm::mat4 projection)
{
    glm::mat4 model(1.0);
    model = glm::translate(model, position);
    model = glm::rotate(model, rotation.x, { 1,0,0 });
    model = glm::rotate(model, rotation.y, { 0,1,0 });
    model = glm::rotate(model, rotation.z, { 0,0,1 });
    model = glm::scale(model, { scale.x,scale.y,scale.z });
    vertex_array.face_normal.Draw(view, projection, model);
}