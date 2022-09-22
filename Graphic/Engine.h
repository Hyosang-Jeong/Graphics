#include<vector>
#include <GL/glew.h> 
#include <GLFW/glfw3.h>
#include"Scene/Scene.h"
class Engine
{
public:
	Engine(GLint w, GLint h);
	~Engine();
	void Add(Scene* scene);
	void init();
	void Update();
	void Draw();
	bool ShouldClose();
	void demo_switch();
private:
	std::vector<Scene*> scenes;
	GLFWwindow* ptr_window;
	int index;
};