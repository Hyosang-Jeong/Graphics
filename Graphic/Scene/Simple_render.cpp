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
    create_shaders();
    create_models();
    create_objects();
    create_lights();
    create_spheres_and_lines();
    load_skybox();
    metal_diff_ppm = load_image("../textures/metal_roof_diff_512x512.png");
    metal_spec_ppm = load_image("../textures/metal_roof_spec_512x512.png");
    setup_skyboxTransform();
    lightUBO.SetUniformBlock(2112, &lights);
    camera = Camera({ 0.0f, 0.f, 8.0f });
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
            diff += deltaTime * 0.1f;
            lights.light[i].UpdatePosition(lightObj[i].position);
        }
    }
    lightUBO.UpdateUniformBlock();

}
void Simple_render::Draw()
{
    Draw_Skybox_to_Framebuffer();
    Draw_Skybox();
    Draw_CenterObj();
    Draw_Lights();
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
        static const char* current_item = model[4];
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
        static const char* shader[] = { "phong_shading","blinn"};
        static  const char* current_shader = shader[0];
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
        if (ImGui::SliderFloat("Camera Yaw", &Yaw, 0, 360.f))
        {
            camera.SetYaw(Yaw);
        }
        float Pitch = camera.GetPitch();
        if (ImGui::SliderFloat("Camera Pitch", &Pitch, -360.f, 0.f))
        {
            camera.SetPitch(Pitch);
        }
    }
    if (ImGui::CollapsingHeader("Material"))
    {
        ImGui::Text("Surface Color Tints");
        static float MaterialEmissive[] = { material.emissive.x,material.emissive.y,material.emissive.z };
        if (ImGui::ColorEdit3("Emissive", MaterialEmissive))
        {
            material.emissive.x = MaterialEmissive[0];
            material.emissive.y = MaterialEmissive[1];
            material.emissive.z = MaterialEmissive[2];
        }

        EnvironmentImGui();
    }

    GlobalImGui();

    LightImGui();
}

void Simple_render::create_shaders()
{
    shaders["phong_shading"] = setup_shdrpgm("phong_shading");
    shaders["blinn"] = setup_shdrpgm("blinn");
    shaders["skybox"] = setup_shdrpgm("skybox");

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

    models[Skybox] = create_cubeMap();
    std::cout << "Load :cubeMap" << std::endl;
}

void Simple_render::create_spheres_and_lines()
{
    const float radius = 4.f;
    for (auto i = 0; i <  maxLight; i++)
    {
       Object obj(shaders["lighting"]);
       obj.position = glm::vec3{ radius * sin(glm::radians(360.f / (float)maxLight * i)), 0, radius * cos(glm::radians(360.f / (float)maxLight * i)) };
       obj.scale = glm::vec3(0.2f);
       obj.model = models[sphere];
       lightObj.push_back(obj);
    }
}
void Simple_render::create_objects()
{

        Object obj(shaders["phong_shading"]);
        obj.position = glm::vec3{ 0,0,0 };
        obj.scale = glm::vec3(1.f);
        obj.model = models[sphere];

        centerObj = obj;
        material.ambient = glm::vec3(0.f,0.f,3.f/255.f);
        material.diffuse = glm::vec3(1.f);
        material.specular = glm::vec3(1.f);
        material.emissive = glm::vec3(0.f);

        Object obj2(shaders["skybox"]);
        obj2.model = models[Skybox];
        SkyBox = obj2;

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
 
void Simple_render::load_skybox()
{
    skyBoxTextures.push_back(load_image("../textures/left.jpg"));
    skyBoxTextures.push_back(load_image("../textures/right.jpg"));
    skyBoxTextures.push_back(load_image("../textures/bottom.jpg"));
    skyBoxTextures.push_back(load_image("../textures/top.jpg"));
    skyBoxTextures.push_back(load_image("../textures/back.jpg"));
    skyBoxTextures.push_back(load_image("../textures/front.jpg"));

    for (int i = 0; i < 6; i++)
    {
        unsigned int texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1600, 1000, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        emptyTextures.push_back(texture);
    }
}

void Simple_render::setup_skyboxTransform()
{
    skyBoxTransform[0] = (glm::perspective(glm::radians(90.0f), 1.f, 0.1f, 100.f) * glm::lookAt(glm::vec3(0),  glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0)));
    skyBoxTransform[1] = (glm::perspective(glm::radians(90.0f), 1.f, 0.1f, 100.f) * glm::lookAt(glm::vec3(0),  glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0,  1.0, 0.0)));
    skyBoxTransform[2] = (glm::perspective(glm::radians(90.0f), 1.f, 0.1f, 100.f) * glm::lookAt(glm::vec3(0),  glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)));
    skyBoxTransform[3] = (glm::perspective(glm::radians(90.0f), 1.f, 0.1f, 100.f) * glm::lookAt(glm::vec3(0),  glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0,-1.0)));
    skyBoxTransform[4] = (glm::perspective(glm::radians(90.0f), 1.f, 0.1f, 100.f) * glm::lookAt(glm::vec3(0),  glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, 1.0, 0.0)));
    skyBoxTransform[5] = (glm::perspective(glm::radians(90.0f), 1.f, 0.1f, 100.f) * glm::lookAt(glm::vec3(0),  glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, 1.0, 0.0)));
}

void Simple_render::reload_shaders(std::string shader)
{
    shaders[shader] = setup_shdrpgm(shader);
    centerObj.Shader_handle = shaders[shader];
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

void Simple_render::EnvironmentImGui()
{
    ImGui::Text("Environment Mapping");
    ImGui::Checkbox("Visualize Reflaction", &flag.UseReflaction);
    ImGui::Checkbox("Visualize Refraction", &flag.UseRefraction);
    static const char* Refraction_Materials[] =
    {  "Air",
       "Hydrogen" ,
       "Water",
       "Olive_Oil",
       "Ice",
       "Quartz",
       "Diamond",
       "Acrylic",
        "plexiglas",
        "Lucite"  
    };
    static  const char* currentName= Refraction_Materials[0];
    static int current = 0;

    static float RefractionMaterialsValue[] = 
    {
         1.0003f,
         1.0001f,
         1.333f,
         1.47f,
         1.31f,
         1.46f,
         2.42f,
         1.49f,
         1.49f,
         1.49f
    };

    refractive_index = RefractionMaterialsValue[current];
    ImGui::ListBox("Refraction Materials", &current, Refraction_Materials, 10);
    ImGui::SliderFloat("Refraction Index", &refractive_index, 1.f, 100.f);
    ImGui::SliderFloat("FresnelPower",&FresnelPower,0.1f,1.f);
    ImGui::SliderFloat("Phong & Environment Ratio", &PhongEnvironmentRatio, 0.f, 1.f);
}

void Simple_render::Draw_CenterObj()
{
        glUseProgram(centerObj.Shader_handle);
    
        GLint texture = glGetUniformLocation(centerObj.Shader_handle, "skybox_LEFT");
        glUniform1i(texture, 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, emptyTextures[0]);

        texture = glGetUniformLocation(centerObj.Shader_handle, "skybox_RIGHT");
        glUniform1i(texture, 1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, emptyTextures[1]);

        texture = glGetUniformLocation(centerObj.Shader_handle, "skybox_BOTTOM");
        glUniform1i(texture, 2);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, emptyTextures[2]);

        texture = glGetUniformLocation(centerObj.Shader_handle, "skybox_TOP");
        glUniform1i(texture, 3);
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, emptyTextures[3]);

        texture = glGetUniformLocation(centerObj.Shader_handle, "skybox_BACK");
        glUniform1i(texture, 4);
        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_2D, emptyTextures[4]);

        texture = glGetUniformLocation(centerObj.Shader_handle, "skybox_FRONT");
        glUniform1i(texture, 5);
        glActiveTexture(GL_TEXTURE5);
        glBindTexture(GL_TEXTURE_2D, emptyTextures[5]);

        GLint diffuse_texture = glGetUniformLocation(centerObj.Shader_handle, "DiffuseMap");
        glUniform1i(diffuse_texture, 10);
        glActiveTexture(GL_TEXTURE10);
        glBindTexture(GL_TEXTURE_2D, metal_diff_ppm);

        diffuse_texture = glGetUniformLocation(centerObj.Shader_handle, "SpecularMap");
        glUniform1i(diffuse_texture, 11);
        glActiveTexture(GL_TEXTURE11);
        glBindTexture(GL_TEXTURE_2D, metal_spec_ppm);

        GLint Uniformloc = glGetUniformLocation(centerObj.Shader_handle, "UseTexture");
        glUniform1i(Uniformloc, true);

        Uniformloc = glGetUniformLocation(centerObj.Shader_handle, "viewPos");
        glUniform3f(Uniformloc, camera.GetPosition().x, camera.GetPosition().y, camera.GetPosition().z);

        Uniformloc = glGetUniformLocation(centerObj.Shader_handle, "material_Ambient");
        glUniform3fv(Uniformloc, 1, &material.ambient.x);

        Uniformloc = glGetUniformLocation(centerObj.Shader_handle, "material_Diffuse");
        glUniform3fv(Uniformloc, 1, &material.diffuse.x);

        Uniformloc = glGetUniformLocation(centerObj.Shader_handle, "material_Specular");
        glUniform3fv(Uniformloc, 1, &material.specular.x);

        Uniformloc = glGetUniformLocation(centerObj.Shader_handle, "material_Emissive");
        glUniform3fv(Uniformloc, 1, &material.emissive.x);

        Uniformloc = glGetUniformLocation(centerObj.Shader_handle, "VisualRefraction");
        glUniform1i(Uniformloc, flag.UseRefraction);

        Uniformloc = glGetUniformLocation(centerObj.Shader_handle, "VisualReflection");
        glUniform1i(Uniformloc, flag.UseReflaction);

        Uniformloc = glGetUniformLocation(centerObj.Shader_handle, "RefractionIndex");
        glUniform1f(Uniformloc, refractive_index);

        Uniformloc = glGetUniformLocation(centerObj.Shader_handle, "FresnelPower");
        glUniform1f(Uniformloc, FresnelPower);

        Uniformloc = glGetUniformLocation(centerObj.Shader_handle, "PhongEnvironmentRatio");
        glUniform1f(Uniformloc, PhongEnvironmentRatio);

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

void Simple_render::Draw_Skybox()
{
        glUseProgram(SkyBox.Shader_handle);
        glDepthMask(GL_FALSE);
        glDisable(GL_DEPTH_TEST);
        GLint texture = glGetUniformLocation(SkyBox.Shader_handle, "skybox_LEFT");
        glUniform1i(texture, 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, skyBoxTextures[0]);

        texture = glGetUniformLocation(SkyBox.Shader_handle, "skybox_RIGHT");
        glUniform1i(texture, 1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, skyBoxTextures[1]);

        texture = glGetUniformLocation(SkyBox.Shader_handle, "skybox_BOTTOM");
        glUniform1i(texture, 2);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, skyBoxTextures[2]);

        texture = glGetUniformLocation(SkyBox.Shader_handle, "skybox_TOP");
        glUniform1i(texture, 3);
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, skyBoxTextures[3]);

        texture = glGetUniformLocation(SkyBox.Shader_handle, "skybox_BACK");
        glUniform1i(texture, 4);
        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_2D, skyBoxTextures[4]);

        texture = glGetUniformLocation(SkyBox.Shader_handle, "skybox_FRONT");
        glUniform1i(texture, 5);
        glActiveTexture(GL_TEXTURE5);
        glBindTexture(GL_TEXTURE_2D, skyBoxTextures[5]);

        GLint Transform_loc = glGetUniformLocation(SkyBox.Shader_handle, "projection");
 
        glm::mat4 tmp = glm::perspective(glm::radians(90.0f), 1.f, 0.1f, 100.f) * glm::mat4(glm::mat3(camera.GetCamera()));
        glUniformMatrix4fv(Transform_loc, 1, GL_FALSE, &tmp[0].x);

        SkyBox.model->Use();
        glDrawElements(GL_TRIANGLES, SkyBox.model->numIndices, GL_UNSIGNED_INT, nullptr);
        glEnable(GL_DEPTH_TEST);
        glDepthMask(GL_TRUE);
}

void Simple_render::Draw_Skybox_to_Framebuffer()
{
    fbo.Use();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    for (int i = 0; i < 6; i++)
    {
        glUseProgram(SkyBox.Shader_handle);
        fbo.AttachTexture(emptyTextures[i]);
    
        GLint texture = glGetUniformLocation(SkyBox.Shader_handle, "skybox_LEFT");
        glUniform1i(texture, 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, skyBoxTextures[0]);

        texture = glGetUniformLocation(SkyBox.Shader_handle, "skybox_RIGHT");
        glUniform1i(texture, 1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, skyBoxTextures[1]);

        texture = glGetUniformLocation(SkyBox.Shader_handle, "skybox_BOTTOM");
        glUniform1i(texture, 2);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, skyBoxTextures[2]);

        texture = glGetUniformLocation(SkyBox.Shader_handle, "skybox_TOP");
        glUniform1i(texture, 3);
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, skyBoxTextures[3]);

        texture = glGetUniformLocation(SkyBox.Shader_handle, "skybox_BACK");
        glUniform1i(texture, 4);
        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_2D, skyBoxTextures[4]);

        texture = glGetUniformLocation(SkyBox.Shader_handle, "skybox_FRONT");
        glUniform1i(texture, 5);
        glActiveTexture(GL_TEXTURE5);
        glBindTexture(GL_TEXTURE_2D, skyBoxTextures[5]);

        GLint Transform_loc = glGetUniformLocation(SkyBox.Shader_handle, "projection");

        glUniformMatrix4fv(Transform_loc, 1, GL_FALSE, &skyBoxTransform[i][0].x);

        SkyBox.model->Use();
        glDrawElements(GL_TRIANGLES, SkyBox.model->numIndices, GL_UNSIGNED_INT, nullptr);

        for (int j = 0; j < lights.activeLight; j++)
        {
            glUseProgram(lightObj[j].Shader_handle);
            GLint light_loc = glGetUniformLocation(lightObj[j].Shader_handle, "diffuse");
            glUniform3f(light_loc, lights.light[j].material.diffuse.x, lights.light[j].material.diffuse.y, lights.light[j].material.diffuse.z);
            lightObj[j].Draw(glm::mat4(1), skyBoxTransform[i]);
        }
    }
      fbo.UnUse();
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

