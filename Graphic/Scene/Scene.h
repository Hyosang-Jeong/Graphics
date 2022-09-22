#pragma once
#include <glm/gtc/matrix_transform.hpp>
class Scene
{
public:
	Scene() = default;
	virtual ~Scene() {};

	virtual void init() = 0;
	virtual void Update(float) = 0;
	virtual void Draw() = 0;
	virtual void OnImGuiRender() = 0;
	virtual void UnLoad() = 0;

protected:
    glm::mat4 projection =  glm::perspective(glm::radians(45.0f), 1.f, 0.1f, 100.0f);
};