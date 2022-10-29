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
#include"Image.h"
#include<iostream>
using namespace glm;
Simple_render::Simple_render() : active_light(1)
{
    metal_diff_ppm = load_ppm("../textures/metal_roof_diff_512x512.ppm");
    metal_spec_ppm = load_ppm("../textures/metal_roof_spec_512x512.ppm");
    create_shaders();
    create_models();
    create_objects();
    create_lights();
    create_spheres_and_lines();
    lightUBO.SetUniformBlock(1552, &lights);
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
    //objects[0].rotation = { 0, objects[0].rotation.y + deltaTime*0.2f,0 };
    float angle = 360.f / (float)maxLight;

    for (int i =0; i< lightObj.size(); i++)
    {
        lightObj[i].position = (glm::vec3{ 4.f * sin(glm::radians(angle * i) + glfwGetTime() * 0.5f), 0, 4.f * cos(glm::radians(angle * i) + glfwGetTime() * 0.5f) });
        lights.light[i].UpdatePosition(lightObj[i].position);
    }
    lightUBO.UpdateUniformBlock();

}

void Simple_render::Draw()
{
    circle.Draw(camera.GetCamera(), projection);
    Draw_CenterObj();
    Draw_Lights();
    Draw_Plane();
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

    static const char* model[] = { "Bunny", "4Sphere", "Cube", "Sphere_modified", "Sphere"};
    static const char* current_item = model[0];
    if (ImGui::BeginCombo("Model", current_item)) // The second parameter is the label previewed before opening the combo.
    {
        for (int n = 0; n < 5; n++)
        {
            bool is_selected = (current_item == model[n]);
            if (ImGui::Selectable(model[n], is_selected))
            {
                current_item = model[n];
                centerObj.model = models[n];
            }
            if (is_selected)
            {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }

   static const char* shader[] = { "phong_lighting", "phong_shading"};
   static  const char* current_shader = shader[1];
    if (ImGui::BeginCombo("Shader", current_shader)) // The second parameter is the label previewed before opening the combo.
    {
        for (int n = 0; n < 2; n++)
        {
            bool is_selected = (current_shader == shader[n]);
            if (ImGui::Selectable(shader[n], is_selected))
            {
                current_shader = shader[n];
            }
            if (is_selected)
            {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }
    if (ImGui::Button("Reload Shader"))
    {
        reload_shaders(current_shader);
    }

    if (flag.draw_vtx_normal)
    {
        centerObj.Draw_vtx_normal(camera.GetCamera(), projection);
    }
    if (flag.draw_face_normal)
    {
        centerObj.Draw_face_normal(camera.GetCamera(), projection);
    }
    glm::vec3 camera_pos = camera.GetPosition();
    if (ImGui::SliderFloat3("Camera position", &camera_pos.x, -20.f, 20.f))
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

    LightImGui();

}

void Simple_render::create_shaders()
{
    shaders["phong_lighting"] = setup_shdrpgm("phong_lighting");
    shaders["phong_shading"] = setup_shdrpgm("phong_shading");
    shaders["lighting"] = setup_shdrpgm("lighting");
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

    models[Plane] = load_obj("../obj/quad.obj");
    std::cout << "Load :plane model" << std::endl;
}

void Simple_render::create_spheres_and_lines()
{
    const float radius = 4.f;
    for (auto i = 0; i <  maxLight; i++)
    {
       Object obj(shaders["lighting"]);
       obj.position = glm::vec3{ radius * sin(glm::radians(360.f / (float)maxLight * i)), 0, radius * cos(glm::radians(360.f / (float)maxLight * i)) };
       obj.scale = glm::vec3(0.2f);
       obj.model = models[Sphere];
       lightObj.push_back(obj);
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
    //plane 
    { 
        Object obj(shaders["phong_shading"]);
        obj.position = glm::vec3{ 0,-4,-4 };
        obj.scale = glm::vec3(8);
        obj.rotation = glm::vec3(-pi<float>() / 2.f, 0, 0);
        obj.model = models[Plane];
        plane = obj;
    }

    {
        Object obj(shaders["phong_shading"]);
        obj.position = glm::vec3{ 0,0,0 };
        obj.scale = glm::vec3(1.f);
        obj.model = models[Bunny];
        centerObj = obj;
    }
}

void Simple_render::create_lights()
{
    for (int i = 0; i < maxLight; i++)
    {
         Light light(LIGHT_TYPE::DIRECTIONAL, Material(), glm::vec3(1));
        lights.light[i] = light;
    }
    lights.activeLight = active_light;
}

void Simple_render::reload_shaders(std::string shader)
{
    centerObj.Shader_handle = shaders[shader];
}

void Simple_render::LightImGui()
{
    ImGui::Begin("Light");
    static float ambient[] = { 1.f,1.f,1.f};
    static float diffuse[] = { 1.f,1.f,1.f };
    static float specular[] = { 1.f,1.f,1.f };

    if (ImGui::ColorEdit3("Ambient", ambient))
    {
        for(int i = 0; i< maxLight; i++)
             lights.light[i].UpdateAmbient(glm::vec3(ambient[0], ambient[1], ambient[2]));
   }
    if (ImGui::ColorEdit3("Diffuse", diffuse))
    {
        for (int i = 0; i < maxLight; i++)
            lights.light[i].UpdateDiffuse(glm::vec3(diffuse[0], diffuse[1], diffuse[2]));
    }   
    if (ImGui::ColorEdit3("Specular", specular))
    {
        for (int i = 0; i < maxLight; i++)
            lights.light[i].UpdateSpecular(glm::vec3(specular[0], specular[1], specular[2]));
    }

    if (ImGui::SliderInt("Active lights", &active_light, 1, maxLight))
    {
        lights.activeLight = active_light;
    }
}

void Simple_render::Draw_CenterObj()
{
        glUseProgram(centerObj.Shader_handle);

        GLint diffuse_texture = glGetUniformLocation(centerObj.Shader_handle, "material_Diffuse");
        glUniform1d(diffuse_texture, 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, metal_diff_ppm);

        diffuse_texture = glGetUniformLocation(centerObj.Shader_handle, "material_Specular");
        glUniform1d(diffuse_texture, 1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, metal_spec_ppm);


        GLint light_loc = glGetUniformLocation(plane.Shader_handle, "UseTexture");
        glUniform1i(light_loc, true);

        lightUBO.Use();
        centerObj.Draw(camera.GetCamera(), projection);
}

void Simple_render::Draw_Lights()
{
    for (int i = 0; i < active_light; i++)
    {
        glUseProgram(lightObj[i].Shader_handle);
        GLint light_loc = glGetUniformLocation(lightObj[i].Shader_handle, "diffuse");
        glUniform3f(light_loc, lights.light[i].material.diffuse.x, lights.light[i].material.diffuse.y, lights.light[i].material.diffuse.z);
        lightObj[i].Draw(camera.GetCamera(), projection);
    }
}

void Simple_render::Draw_Plane()
{
    glUseProgram(plane.Shader_handle);
    
    GLint light_loc = glGetUniformLocation(plane.Shader_handle, "UseTexture");
    glUniform1i(light_loc, false);

    lightUBO.Use();
    plane.Draw(camera.GetCamera(), projection);
}

