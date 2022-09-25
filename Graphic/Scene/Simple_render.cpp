#include "Simple_render.h"
#include<GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp> //translate, rotate, scale, perspective 
#include <imgui.h>

#include <imgui_impl_opengl3.h>
#include <imgui_impl_glfw.h>

using namespace glm;
Simple_render::Simple_render()
{
    models[Bunny] = load_obj("../obj/bunny_high_poly.obj");
    models[_4Sphere] =load_obj("../obj/4Sphere.obj");
    models[Cube] = load_obj("../obj/cube2.obj");
    models[Sphere] = create_sphere(32, 32);

    create_objects();
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
    objects[model_to_draw].rotation = { 0, objects[model_to_draw].rotation.y + deltaTime*0.2f,0 };
    float angle = 360.f / 8.f;

    for (int i = 0; i < objects.size(); i++)
    {
        if(objects[i].model_to_use == Sphere)
            objects[i].position = (glm::vec3{ 4.f * sin(glm::radians(angle * i) + glfwGetTime() * 0.5f), 0, 4.f * cos(glm::radians(angle * i)  +glfwGetTime() * 0.5f) });
    }
}

void Simple_render::Draw()
{
    circle.Draw(camera.GetCamera(), projection);

    {
        glUseProgram(objects[model_to_draw].Shader_handle);
        GLint light_loc = glGetUniformLocation(objects[model_to_draw].Shader_handle, "lightPos");
        glUniform3f(light_loc, light_pos.x, light_pos.y, light_pos.z);
        objects[model_to_draw].Draw(models[objects[model_to_draw].model_to_use], camera.GetCamera(), projection);
    }

    for (int i = 3; i<objects.size(); i++)
    {
        glUseProgram(objects[i].Shader_handle);
        GLint light_loc = glGetUniformLocation(objects[i].Shader_handle, "lightPos");
        glUniform3f(light_loc, light_pos.x, light_pos.y, light_pos.z);
        objects[i].Draw(models[objects[i].model_to_use], camera.GetCamera(), projection);
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

    const char* items[] = { "Bunny", "4Sphere", "Cube"};
    static const char* current_item = items[0];

    if (ImGui::BeginCombo("Model", current_item)) // The second parameter is the label previewed before opening the combo.
    {
        for (int n = 0; n < 3; n++)
        {
            bool is_selected = (current_item == items[n]);
            if (ImGui::Selectable(items[n], is_selected))
            {
                current_item = items[n];
                model_to_draw = n;
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
        objects[model_to_draw].Draw_vtx_normal(models[objects[model_to_draw].model_to_use], camera.GetCamera(), projection);
    }
    if (flag.draw_face_normal)
    {
        objects[model_to_draw].Draw_face_normal(models[objects[model_to_draw].model_to_use], camera.GetCamera(), projection);
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

void Simple_render::create_spheres_and_lines()
{
    const float radius = 4.f;

    for (auto i = 0; i <  8; i++)
    {
       Object obj("simple_render");
       obj.position = glm::vec3{ radius * sin(glm::radians(360.f / 8.f * i)), 0, radius * cos(glm::radians(360.f / 8.f * i)) };
       obj.scale = glm::vec3(0.2f);
       obj.model_to_use = Sphere;
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
    { Object obj("simple_render");
    obj.position = glm::vec3{ 0,0,0 };
    obj.scale = glm::vec3(1.f);
    obj.model_to_use = Bunny;
    objects.push_back(obj);
    }
    {
        Object obj("simple_render");
        obj.position = glm::vec3{ 0,0,0 };
        obj.scale = glm::vec3(1.f);
        obj.model_to_use = _4Sphere;
        objects.push_back(obj);
    }
    {
    Object obj("simple_render");
    obj.position = glm::vec3{ 0,0,0 };
    obj.scale = glm::vec3(1.f);
    obj.model_to_use = Cube;
    objects.push_back(obj);
    }
}