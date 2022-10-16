/* Start Header -------------------------------------------------------
Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: Simple_render.cpp
Purpose: First demo scene
Language: c++ Microsoft Visual Studio
Platform: Microsoft Visual Studio2019, Windows
Project:  Hyosang Jung_CS300_1
Author: Hyosang Jung, hyosang.jung, 055587
Creation date: 2022 - 09 - 15
End Header --------------------------------------------------------*/
#include "Simple_render.h"
#include<GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp> //translate, rotate, scale, perspective 
#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_glfw.h>
#include<iostream>
using namespace glm;
Simple_render::Simple_render()
{
    create_models();
    create_objects();
    create_spheres_and_lines();
    light_pos = { 0,0,10 };
    camera = Camera({ 0.0f, 5.0f, 10.0f });
    camera.SetPitch(-25.f);
}

Simple_render::~Simple_render()
{
    for (auto& i : models)
    {
        delete i;
    }
}



void Simple_render::Update(float deltaTime)
{
    objects[0].rotation = { 0, objects[0].rotation.y + deltaTime*0.2f,0 };
    float angle = 360.f / 8.f;

    for (int i = 0; i < objects.size(); i++)
    {
        if(objects[i].model == models[Sphere])
            objects[i].position = (glm::vec3{ 4.f * sin(glm::radians(angle * i) + glfwGetTime() * 0.5f), 0, 4.f * cos(glm::radians(angle * i)  +glfwGetTime() * 0.5f) });
    }
}

void Simple_render::Draw()
{
    circle.Draw(camera.GetCamera(), projection);

    {
        glUseProgram(objects[0].Shader_handle);
        GLint light_loc = glGetUniformLocation(objects[0].Shader_handle, "lightPos");
        glUniform3f(light_loc, light_pos.x, light_pos.y, light_pos.z);
        objects[0].Draw(camera.GetCamera(), projection);
    }

    for (int i = 1; i<objects.size(); i++)
    {
        glUseProgram(objects[i].Shader_handle);
        GLint light_loc = glGetUniformLocation(objects[i].Shader_handle, "lightPos");
        glUniform3f(light_loc, light_pos.x, light_pos.y, light_pos.z);
        objects[i].Draw(camera.GetCamera(), projection);
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

    const char* items[] = { "Bunny", "4Sphere", "Cube", "Sphere_modified", "Sphere"};
    static const char* current_item = items[0];

    if (ImGui::BeginCombo("Model", current_item)) // The second parameter is the label previewed before opening the combo.
    {
        for (int n = 0; n < 5; n++)
        {
            bool is_selected = (current_item == items[n]);
            if (ImGui::Selectable(items[n], is_selected))
            {
                current_item = items[n];
                objects[0].model = models[n];
            }
            if (is_selected)
            {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }

    if (flag.draw_vtx_normal)
    {
        objects[0].Draw_vtx_normal(camera.GetCamera(), projection);
    }
    if (flag.draw_face_normal)
    {
        objects[0].Draw_face_normal(camera.GetCamera(), projection);
    }
    ImGui::SliderFloat3("Light position", &light_pos.x, -20.f, 20.f);

    glm::vec3 camera_pos = camera.GetPosition();
    if (ImGui::SliderFloat3("Camera position", &camera_pos.x, -10.f, 10.f))
    {
        camera.SetPosition(camera_pos);
    }

    float Yaw = camera.GetYaw();
    if (ImGui::SliderFloat("Camera Yaw", &Yaw, -125.f, -45.f))
    {
        camera.SetYaw(Yaw);
    }

    float Pitch = camera.GetPitch();
    if (ImGui::SliderFloat("Camera Pitch", &Pitch, -50.f, 0.f))
    {
        camera.SetPitch(Pitch);
    }
}

void Simple_render::create_models()
{
    models[Bunny] = load_obj("../obj/bunny_high_poly.obj");
    std::cout << "Load :bunny_high_poly.obj" << std::endl;

    models[_4Sphere] = load_obj("../obj/4Sphere.obj");
    std::cout << "Load :4Sphere.obj" << std::endl;

    models[Cube] = load_obj("../obj/cube2.obj");
    std::cout << "Load :cube2.obj" << std::endl;

    models[sphere_modified] = load_obj("../obj/sphere_modified.obj");
    std::cout << "Load :sphere_modified.obj" << std::endl;

    models[sphere] = load_obj("../obj/sphere.obj");
    std::cout << "Load :sphere.obj" << std::endl;

    models[Sphere] = create_sphere(32, 32);
    std::cout << "Load :sphere model" << std::endl;
}

void Simple_render::create_spheres_and_lines()
{
    const float radius = 4.f;

    for (auto i = 0; i <  8; i++)
    {
       Object obj("simple_render");
       obj.position = glm::vec3{ radius * sin(glm::radians(360.f / 8.f * i)), 0, radius * cos(glm::radians(360.f / 8.f * i)) };
       obj.scale = glm::vec3(0.2f);
       obj.model = models[Sphere];
       objects.push_back(obj);
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
void Simple_render::create_objects()
{
         Object obj("simple_render");
         obj.position = glm::vec3{ 0,0,0 };
         obj.scale = glm::vec3(1.f);
         obj.model = models[Bunny];
         objects.push_back(obj);
}