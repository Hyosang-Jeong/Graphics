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
Simple_render::Simple_render() 
{
    metal_diff_ppm = load_ppm("../textures/metal_roof_diff_512x512.ppm");
    metal_spec_ppm = load_ppm("../textures/metal_roof_spec_512x512.ppm");
    create_shaders();
    create_models();
    create_objects();
    create_lights();
    create_spheres_and_lines();
    lightUBO.SetUniformBlock(2112, &lights);
    camera = Camera({ 0.0f, 7.5f, 20.0f });
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
    float angle = 360.f / (float)lights.activeLight;
    static float diff = 0;
    if (flag.rotation)
    {
        for (int i = 0; i < lightObj.size(); i++)
        {
            lightObj[i].position = (glm::vec3{ 4.f * sin(glm::radians(angle * i) + diff), 0 , 4.f * cos(glm::radians(angle * i) + diff) });
            diff += deltaTime*0.1f;
            lights.light[i].UpdatePosition(lightObj[i].position);
        }
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
    ImGui::Begin("Controls");
    if (ImGui::CollapsingHeader("Model Control"))
    {
        ImGui::Checkbox("Draw Vertex normal", &flag.draw_vtx_normal);
        ImGui::Checkbox("Draw Face normal", &flag.draw_face_normal);
        static const char* model[] = { "Bunny", "4Sphere", "Cube", "Sphere_modified", "Sphere" };
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
    }
    if (ImGui::CollapsingHeader("Shader Control"))
    {
        static const char* shader[] = { "phong_lighting", "phong_shading","blinn"};
        static  const char* current_shader = shader[1];
        if (ImGui::BeginCombo("Shader", current_shader)) // The second parameter is the label previewed before opening the combo.
        {
            for (int n = 0; n < 3; n++)
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

    if (ImGui::CollapsingHeader("Camera"))
    {
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
    }
    if (ImGui::CollapsingHeader("Material"))
    {
        ImGui::Text("Surface Color Tints");
        static float MaterialAmbient[] = { material.ambient.x,material.ambient.y,material.ambient.z };
        static float MaterialDiffuse[] = { material.diffuse.x,material.diffuse.y,material.diffuse.z };
        static float MaterialSpecular[] = { material.specular.x,material.specular.y,material.specular.z };
        static float MaterialEmissive[] = { material.emissive.x,material.emissive.y,material.emissive.z };
        if (ImGui::ColorEdit3("Ambient", MaterialAmbient))
        {
            material.ambient.x = MaterialAmbient[0];
            material.ambient.y = MaterialAmbient[1];
            material.ambient.z = MaterialAmbient[2];
        }
        if (ImGui::ColorEdit3("Diffuse", MaterialDiffuse))
        {
            material.diffuse.x = MaterialDiffuse[0];
            material.diffuse.y = MaterialDiffuse[1];
            material.diffuse.z = MaterialDiffuse[2];
        }
        if (ImGui::ColorEdit3("Specular", MaterialSpecular))
        {
            material.specular.x = MaterialSpecular[0];
            material.specular.y = MaterialSpecular[1];
            material.specular.z = MaterialSpecular[2];
        }
        if (ImGui::ColorEdit3("Emissive", MaterialEmissive))
        {
            material.emissive.x = MaterialEmissive[0];
            material.emissive.y = MaterialEmissive[1];
            material.emissive.z = MaterialEmissive[2];
        }


        static const char* projection[] = { "Cylindrical", "Spherical","Cube"};
        static  const char* current_projection = projection[0];
        static const char* entities[] = { "Position", "Normal" };
        static  const char* current_entity = entities[0];
        if (ImGui::BeginCombo("Texture projection mode", current_projection)) // The second parameter is the label previewed before opening the combo.
        {
            for (int n = 0; n < 3; n++)
            {
                bool is_selected = (current_projection == projection[n]);
                if (ImGui::Selectable(projection[n], is_selected))
                {
                    current_projection = projection[n];
                    flag.projectionMode = n;
                    switch (n)
                    {
                    case 0: centerObj.model->Calculate_uv_cylindrical(current_entity == entities[0]?true:false);
                        break;
                    case 1: centerObj.model->Calculate_uv_spherical(current_entity == entities[0] ? true : false);
                        break;
                    case 2: centerObj.model->Calculate_uv_planar(current_entity == entities[0] ? true : false);
                        break;
                    default:
                        break;
                    }
                }
                if (is_selected)
                {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }

        if (ImGui::BeginCombo("Texture Entity", current_entity)) // The second parameter is the label previewed before opening the combo.
        {
            for (int n = 0; n < 2; n++)
            {
                bool is_selected = (current_entity == entities[n]);
                if (ImGui::Selectable(entities[n], is_selected))
                {
                    current_entity = entities[n];
                    flag.UsePosTOuv = (current_entity == entities[0] ? true : false);
                    switch (flag.projectionMode)
                    {
                    case 0: centerObj.model->Calculate_uv_cylindrical(current_entity == entities[0] ? true : false);
                        break;
                    case 1: centerObj.model->Calculate_uv_spherical(current_entity == entities[0] ? true : false);
                        break;
                    case 2: centerObj.model->Calculate_uv_planar(current_entity == entities[0] ? true : false);
                        break;
                    default:
                        break;
                    }
                }
                if (is_selected)
                {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }


        static const char* UV[] = { "CPU", "GPU" };
        static  const char* current_UV = UV[1];
        if (ImGui::BeginCombo("Calculation in", current_UV)) // The second parameter is the label previewed before opening the combo.
        {
            for (int n = 0; n < 2; n++)
            {
                bool is_selected = (current_UV == UV[n]);
                if (ImGui::Selectable(UV[n], is_selected))
                {
                    current_UV = UV[n];
                    flag.calculateUV_GPU = n;
                }
                if (is_selected)
                {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }



    }


    GlobalImGui();

    LightImGui();
}

void Simple_render::create_shaders()
{
    shaders["phong_lighting"] = setup_shdrpgm("phong_lighting");
    shaders["phong_shading"] = setup_shdrpgm("phong_shading");
    shaders["blinn"] = setup_shdrpgm("blinn");
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
        obj.position = glm::vec3{ 0,-1,0 };
        obj.scale = glm::vec3(10);
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
        material.ambient = glm::vec3(0.f,0.f,3.f/255.f);
        material.diffuse = glm::vec3(1.f);
        material.specular = glm::vec3(1.f);
        material.emissive = glm::vec3(0.f);
    }
}

void Simple_render::create_lights()
{
    for (int i = 0; i < maxLight; i++)
    {
         Light light(LIGHT_TYPE::POINT, Material(), glm::vec3(1), glm::vec3(15.f, 20.f, 1.f));
         lights.light[i] = light;
    }
    lights.activeLight = 1;
}

void Simple_render::reload_shaders(std::string shader)
{
    centerObj.Shader_handle = shaders[shader];
    plane.Shader_handle = shaders[shader];
}

void Simple_render::LightImGui()
{
    ImGui::Begin("Light");

    static const char* Lights[] =
    { "Light#1",
       "Light#2" ,
       "Light#3",
       "Light#4",
       "Light#5",
       "Light#6",
       "Light#7",
       "Light#8",
       "Light#9",
       "Light#10",
       "Light#11",
       "Light#12",
       "Light#13",
       "Light#14",
       "Light#15",
        "Light#16"
    };
    static  const char* current_light = Lights[0];
    static int current = 0;
    if (ImGui::BeginCombo("Select Light", current_light)) // The second parameter is the label previewed before opening the combo.
    {
        for (int n = 0; n < 16; n++)
        {
            bool is_selected = (current_light == Lights[n]);
            if (ImGui::Selectable(Lights[n], is_selected))
            {
                current_light = Lights[n];
                current = n;
            }
            if (is_selected)
            {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }

    static const char* type[] = { "Point", "Direction","SpotLight" };
    const char* current_type = type[static_cast<int>(lights.light[current].type)];

    if (ImGui::BeginCombo("Point type", current_type)) // The second parameter is the label previewed before opening the combo.
    {
        for (int n = 0; n < 3; n++)
        {
            bool is_selected = (current_type == type[n]);
            if (ImGui::Selectable(type[n], is_selected))
            {
                current_type = type[n];
                lights.light[current].type = static_cast<LIGHT_TYPE>(n);
            }
            if (is_selected)
            {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }


    float ambient[] =
    { lights.light[current].material.ambient.x,
       lights.light[current].material.ambient.y,
       lights.light[current].material.ambient.z
    };
    float diffuse[] =
    { lights.light[current].material.diffuse.x,
       lights.light[current].material.diffuse.y,
       lights.light[current].material.diffuse.z
    };
    float specular[] =
    { lights.light[current].material.specular.x,
       lights.light[current].material.specular.y,
       lights.light[current].material.specular.z
    };



    if (ImGui::ColorEdit3("Ambient", ambient))
    {
        lights.light[current].UpdateAmbient(glm::vec3(ambient[0], ambient[1], ambient[2]));
    }
    if (ImGui::ColorEdit3("Diffuse", diffuse))
    {
        lights.light[current].UpdateDiffuse(glm::vec3(diffuse[0], diffuse[1], diffuse[2]));
    }
    if (ImGui::ColorEdit3("Specular", specular))
    {
        lights.light[current].UpdateSpecular(glm::vec3(specular[0], specular[1], specular[2]));
    }

    if (lights.light[current].type == LIGHT_TYPE::SPOT)
    {
        ImGui::SliderFloat("Inner angle", &lights.light[current].Inner_Outer_Falloff.x, 0.f, 90.f);
        lights.light[current].Inner_Outer_Falloff.x = lights.light[current].Inner_Outer_Falloff.x < lights.light[current].Inner_Outer_Falloff.y ?
            lights.light[current].Inner_Outer_Falloff.x : lights.light[current].Inner_Outer_Falloff.y;
        ImGui::SliderFloat("Outer angle", &lights.light[current].Inner_Outer_Falloff.y, 0.f, 90.f);
        lights.light[current].Inner_Outer_Falloff.y = lights.light[current].Inner_Outer_Falloff.y > lights.light[current].Inner_Outer_Falloff.x ?
            lights.light[current].Inner_Outer_Falloff.y : lights.light[current].Inner_Outer_Falloff.x;

        ImGui::SliderFloat("Falloff", &lights.light[current].Inner_Outer_Falloff.z, 0, 10.f);

    }

    if (ImGui::SliderInt("Active lights", &lights.activeLight, 1, maxLight))
    {
        float angle = 360.f / (float)lights.activeLight;
            for (int i = 0; i < lightObj.size(); i++)
            {
                lightObj[i].position = (glm::vec3{ 4.f * sin(glm::radians(angle * i)), 0 , 4.f * cos(glm::radians(angle * i) ) });
  
                lights.light[i].UpdatePosition(lightObj[i].position);
            }
    }

    if (ImGui::Button("Scenario_1"))
    {
        Scenario1();
    }
    ImGui::SameLine();
    if (ImGui::Button("Scenario_2"))
    {
        Scenario2();
    }
    ImGui::SameLine();
    if (ImGui::Button("Scenario_3"))
    {
        Scenario3();
    }
    if (ImGui::Button("Rotation lights"))
    {
        flag.rotation = !flag.rotation;
    }
}

void Simple_render::GlobalImGui()
{
    if (ImGui::CollapsingHeader("Global Constant"))
    {
        ImGui::SliderFloat3("Attenuation constant", &lights.coefficients.x, 0, 2);
        static float globalAmbient[] = { lights.GlobalAmbient.x,lights.GlobalAmbient.y,lights.GlobalAmbient.z };
        static float fogColor[] = { 1.f,1.f,1.f };
        if (ImGui::ColorEdit3("Global Ambient", globalAmbient))
        {
            lights.GlobalAmbient.x = globalAmbient[0];
            lights.GlobalAmbient.y = globalAmbient[1];
            lights.GlobalAmbient.z = globalAmbient[2];
        }
        if (ImGui::ColorEdit3("Fog Color", fogColor))
        {
            lights.fogColor.x = fogColor[0];
            lights.fogColor.y = fogColor[1];
            lights.fogColor.z = fogColor[2];
        }
        ImGui::SliderFloat("Fog Min", &lights.fog_near, 10.f,20.f);
        ImGui::SliderFloat("Fog Max", &lights.fog_far, 40.f, 50.f);
    }
    ImGui::End();
}

void Simple_render::Draw_CenterObj()
{
        glUseProgram(centerObj.Shader_handle);

        GLint diffuse_texture = glGetUniformLocation(centerObj.Shader_handle, "DiffuseMap");
        glUniform1d(diffuse_texture, 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, metal_diff_ppm);

        diffuse_texture = glGetUniformLocation(centerObj.Shader_handle, "SpecularMap");
        glUniform1d(diffuse_texture, 1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, metal_spec_ppm);

        GLint light_loc = glGetUniformLocation(centerObj.Shader_handle, "UseTexture");
        glUniform1i(light_loc, true);

        light_loc = glGetUniformLocation(centerObj.Shader_handle, "viewPos");
        glUniform3f(light_loc, camera.GetPosition().x, camera.GetPosition().y, camera.GetPosition().z);

        light_loc = glGetUniformLocation(centerObj.Shader_handle, "material_Ambient");
        glUniform3fv(light_loc, 1, &material.ambient.x);

        light_loc = glGetUniformLocation(centerObj.Shader_handle, "material_Diffuse");
        glUniform3fv(light_loc, 1, &material.diffuse.x);

        light_loc = glGetUniformLocation(centerObj.Shader_handle, "material_Specular");
        glUniform3fv(light_loc, 1, &material.specular.x);

        light_loc = glGetUniformLocation(centerObj.Shader_handle, "material_Emissive");
        glUniform3fv(light_loc, 1, &material.emissive.x);

        light_loc = glGetUniformLocation(centerObj.Shader_handle, "UseUVFromGPU");
        glUniform1i(light_loc, flag.calculateUV_GPU);

        light_loc = glGetUniformLocation(centerObj.Shader_handle, "UVprojectionType");
        glUniform1i(light_loc, flag.projectionMode);

        light_loc = glGetUniformLocation(centerObj.Shader_handle, "UsePosTOuv");
        glUniform1i(light_loc, flag.UsePosTOuv);

        lightUBO.Use();
        centerObj.Draw(camera.GetCamera(), projection);
}

void Simple_render::Draw_Lights()
{
    for (int i = 0; i < lights.activeLight; i++)
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

    light_loc = glGetUniformLocation(plane.Shader_handle, "viewPos");
    glUniform3f(light_loc, camera.GetPosition().x, camera.GetPosition().y, camera.GetPosition().z);

    lightUBO.Use();
    plane.Draw(camera.GetCamera(), projection);
}

void Simple_render::Scenario1()
{
    lights.activeLight = 6;
    for (int i = 0; i < 6; i++)
    {
        lights.light[i].type = LIGHT_TYPE::POINT;
        lights.light[i].material.ambient =glm::vec3(1.f);
        lights.light[i].material.diffuse = glm::vec3(1.f);
        lights.light[i].material.specular = glm::vec3(1.f);
    }
}

void Simple_render::Scenario2()
{
    lights.activeLight = 7;
    glm::vec3 colors[7] =
    {
        {59.f,234.f,21.f},
        {22.f,89.f,248.f},
        {244.f,14.f,14.f},
        {203.f,29.f,226.f},
        {29.f,226.f,100.f},
        {226.f,215.f,29.f},
        {226.f,184.f,29.f}
    };
    for (int i = 0; i < 7; i++)
    {
        lights.light[i].type = LIGHT_TYPE::SPOT;
        lights.light[i].material.ambient = colors[i] / 255.f;
        lights.light[i].material.diffuse = colors[i] / 255.f;
        lights.light[i].material.specular = colors[i] / 255.f;
    }
}

void Simple_render::Scenario3()
{
    lights.activeLight = 10;
    glm::vec3 colors[10] =
    {
        {59.f,234.f,21.f},
        {22.f,89.f,248.f},
        {226.f,104.f,29.f},
        {283.f,29.f,226.f},
        {226.f,215.f,29.f},
        {29.f,226.f,100.f},
        {203.f,29.f,226.f},
        {203.f,29.f,226.f},
        {28.f,226.f,180.f},
        {226.f,29.f,204.f},
    };
    lights.light[0].type = LIGHT_TYPE::POINT;
    lights.light[0].material.ambient = colors[0] / 255.f;
    lights.light[0].material.diffuse    = colors[0] / 255.f;
    lights.light[0].material.specular = colors[0] / 255.f;

    lights.light[1].type = LIGHT_TYPE::DIRECTIONAL;
    lights.light[1].material.ambient = colors[1] / 255.f;
    lights.light[1].material.diffuse    = colors[1] / 255.f;
    lights.light[1].material.specular = colors[1] / 255.f;

    lights.light[2].type = LIGHT_TYPE::SPOT;
    lights.light[2].material.ambient = colors[2] / 255.f;
    lights.light[2].material.diffuse    = colors[2] / 255.f;
    lights.light[2].material.specular = colors[2] / 255.f;

    lights.light[3].type = LIGHT_TYPE::POINT;
    lights.light[3].material.ambient = colors[3] / 255.f;
    lights.light[3].material.diffuse    = colors[3] / 255.f;
    lights.light[3].material.specular = colors[3] / 255.f;

    lights.light[4].type = LIGHT_TYPE::SPOT;
    lights.light[4].material.ambient = colors[4] / 255.f;
    lights.light[4].material.diffuse =    colors[4] / 255.f;
    lights.light[4].material.specular = colors[4] / 255.f;

    lights.light[5].type = LIGHT_TYPE::SPOT;
    lights.light[5].material.ambient = colors[5] / 255.f;
    lights.light[5].material.diffuse    = colors[5] / 255.f;
    lights.light[5].material.specular = colors[5] / 255.f;

    lights.light[6].type = LIGHT_TYPE::POINT;
    lights.light[6].material.ambient = colors[6] / 255.f;
    lights.light[6].material.diffuse    = colors[6] / 255.f;
    lights.light[6].material.specular = colors[6] / 255.f;

    lights.light[7].type = LIGHT_TYPE::SPOT;
    lights.light[7].material.ambient = colors[7] / 255.f;
    lights.light[7].material.diffuse    = colors[7] / 255.f;
    lights.light[7].material.specular = colors[7] / 255.f;

    lights.light[8].type = LIGHT_TYPE::SPOT;
    lights.light[8].material.ambient = colors[8] / 255.f;
    lights.light[8].material.diffuse    = colors[8] / 255.f;
    lights.light[8].material.specular = colors[8] / 255.f;

    lights.light[9].type = LIGHT_TYPE::POINT;
    lights.light[9].material.ambient = colors[9] / 255.f;
    lights.light[9].material.diffuse    = colors[9] / 255.f;
    lights.light[9].material.specular = colors[9] / 255.f;
}

