/* Start Header -------------------------------------------------------
Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: Simple_render.h
Purpose: First demo scene
Language: c++ Microsoft Visual Studio
Platform: Microsoft Visual Studio2019, Windows
Project:  Hyosang Jung_CS300_1
Author: Hyosang Jung, hyosang.jung, 055587
Creation date: 2022 - 09 - 15
End Header --------------------------------------------------------*/
#pragma once
#include"Scene.h"
#include"Model.h"
#include"Object.h"
#include"Camera.h"
struct ImGui_bool
{
	bool draw_vtx_normal{ false };
	bool draw_face_normal{ false };
};

enum  model_name
{
	Bunny,
	_4Sphere,
	Cube,
	sphere_modified,
	sphere, //from obj file
	Sphere, //from code
	count
};
class Simple_render : public Scene
{
public:
	Simple_render();
	~Simple_render();
	void init() override {}
	void Update(float deltaTime)  override;
	void Draw() override;
	void UnLoad() override;
	void OnImGuiRender() override;

private:
	void create_models();
	void create_spheres_and_lines();
	void create_objects();
	Camera camera;

	Model* models[count];
	std::vector<Object> objects;

	Line circle;
	glm::vec3 light_pos;
	ImGui_bool flag;
};