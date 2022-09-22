#include "Simple_render.h"
#include<GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp> //translate, rotate, scale, perspective 
#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_glfw.h>

using namespace glm;
Simple_render::Simple_render()
{
     models.push_back(load_obj("../obj/4Sphere.obj"));
     models.push_back(load_obj("../obj/cube2.obj"));
    models[0].init("simple_render");
    models[1].init("simple_render");

    create_spheres_and_lines();
    light_pos = { 0,0,10 };
    camera = Camera({ 0.0f, 5.0f, 10.0f });
    camera.SetPitch(-25.f);
}

Simple_render::~Simple_render()
{
}

void Simple_render::init()
{

}

void Simple_render::Update(float deltaTime)
{
    models[0].set_rotation({ 0,models[0].Get_Rotation().y + deltaTime*0.2f,0 });

    static float angle = 360.f / 8.f;

    for (int i = 2; i < 10; i++)
    {
        models[i].set_position(glm::vec3{ 4.f * sin(glm::radians(angle * i) +glfwGetTime()*0.5f),0,4.f * cos(glm::radians(angle * i)  +glfwGetTime() * 0.5f) });
    }
}

void Simple_render::Draw()
{
    {
        glUseProgram(models[0].Shader_handle);
        GLint light_loc = glGetUniformLocation(models[0].Shader_handle, "lightPos");
        glUniform3f(light_loc, light_pos.x, light_pos.y, light_pos.z);
        models[0].Draw(camera.GetCamera(), projection);
    }

    {
        glUseProgram(models[1].Shader_handle);
        GLint light_loc = glGetUniformLocation(models[1].Shader_handle, "lightPos");
        glUniform3f(light_loc, light_pos.x, light_pos.y, light_pos.z);
        models[1].Draw(camera.GetCamera(), projection);
    }

    circle.Draw(camera.GetCamera(), projection);

    for (int i = 2; i < 10; i++)
    {
        glUseProgram(models[i].Shader_handle);
        GLint light_loc = glGetUniformLocation(models[i].Shader_handle, "lightPos");
        glUniform3f(light_loc, light_pos.x, light_pos.y, light_pos.z);
        models[i].Draw(camera.GetCamera(), projection);
    }
    OnImGuiRender();
}

void Simple_render::UnLoad()
{
}

void Simple_render::OnImGuiRender()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    ImGui::Begin("Graphic");
    ImGui::Checkbox("Draw Vertex normal", &flag.draw_vtx_normal);
    ImGui::Checkbox("Draw Face normal", &flag.draw_face_normal);

    if(flag.draw_vtx_normal)
        models[0].Draw_vtx_normal(camera.GetCamera(), projection);
    if (flag.draw_face_normal)
        models[0].Draw_face_normal(camera.GetCamera(), projection);
    ImGui::SliderFloat3("Light position", &light_pos.x, -20.f, 20.f);

    glm::vec3 camera_pos = camera.GetPosition();
    if (ImGui::SliderFloat3("Camera position", &camera_pos.x, -20.f, 20.f))
    {
        camera.SetPosition(camera_pos);
    }

    float Yaw = camera.GetYaw();
    if (ImGui::SliderFloat("Camera Yaw", &Yaw, -360.f, 360.f))
    {
        camera.SetYaw(Yaw);
    }

    float Pitch = camera.GetPitch();
    if (ImGui::SliderFloat("Camera Pitch", &Pitch, -360.f, 360.f))
    {
        camera.SetPitch(Pitch);
    }
}

void Simple_render::create_spheres_and_lines()
{
    float radius = 4.f;
    const auto size = models.size();
    for (auto i = size; i < size + 8; i++)
    {
       models.push_back(create_sphere(32, 32));
       models[i].init("simple_render");
       models[i].set_position(glm::vec3{ radius * sin(glm::radians(360.f / 8.f * i)),0,radius * cos(glm::radians(360.f / 8.f * i)) });
       models[i].set_scale(0.2f);
    }
    std::vector<vec3> vertices;
    int num_points = 720;
    float angle = 360.f / static_cast<float>(num_points);

    for (int i = 0; i < num_points; i++)
    {
        vertices.push_back({ radius * cos(glm::radians(angle * static_cast<float>(i))) , 0 ,radius * sin(glm::radians(angle * static_cast<float>(i))) });
        vertices.push_back({ radius * cos(glm::radians(angle * static_cast<float>(i + 1))) , 0 ,radius * sin(glm::radians(angle * static_cast<float>(i + 1))) });
    }
    circle.init(vertices);
}
