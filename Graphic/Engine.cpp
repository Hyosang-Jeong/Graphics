/* Start Header -------------------------------------------------------
Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: Engine.cpp
Purpose: Engine for running this program
Language: c++ Microsoft Visual Studio
Platform: Microsoft Visual Studio2019, Windows
Project:  Hyosang Jung_CS300_1
Author: Hyosang Jung, hyosang.jung, 055587
Creation date: 2022 - 09 - 10
End Header --------------------------------------------------------*/
#include"Engine.h"
#include<iostream>
#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_glfw.h>
Engine::Engine(GLint w, GLint h)
{
    if (!glfwInit())
    {
        std::cout << "Unable to create OpenGL context" << std::endl;
        std::exit(EXIT_FAILURE);
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);
    glfwWindowHint(GLFW_RED_BITS, 8); glfwWindowHint(GLFW_GREEN_BITS, 8);
    glfwWindowHint(GLFW_BLUE_BITS, 8); glfwWindowHint(GLFW_ALPHA_BITS, 8);
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE); // window dimensions are static

    ptr_window = glfwCreateWindow(w, h, "Graphics", NULL, NULL);

    if (!ptr_window) {
        std::cerr << "GLFW unable to create OpenGL context - abort program\n";
        glfwTerminate();
        std::exit(EXIT_FAILURE);
    }
    glfwMakeContextCurrent(ptr_window);
    glfwSetInputMode(ptr_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    glewInit();
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGui_ImplGlfw_InitForOpenGL(ptr_window, true);
    const char* glsl_version = "#version 450";
    ImGui_ImplOpenGL3_Init(glsl_version);
    ImGui::StyleColorsDark();

    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glCullFace(GL_BACK);

    index = 0;
}

Engine::~Engine()
{
    glfwTerminate();
    for (auto& i : scenes)
    {
        delete i;
        i = nullptr;
    }
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void Engine::Add(Scene* demo)
{
    scenes.push_back(demo);
}

void Engine::init()
{
    scenes[index]->init();
}

void Engine::Update()
{
    glfwPollEvents();
    static double prev_time = glfwGetTime();
    double curr_time = glfwGetTime();
    double delta_time = curr_time - prev_time;
    prev_time = curr_time;

    scenes[index]->Update(static_cast<float>(delta_time));
}

void Engine::Draw()
{
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    scenes[index]->Draw();
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    glfwSwapBuffers(ptr_window);
}

bool Engine::ShouldClose()
{
    return glfwWindowShouldClose(ptr_window);
}

void Engine::demo_switch()
{

}