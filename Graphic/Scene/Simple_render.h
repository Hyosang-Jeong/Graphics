#include"Scene.h"
#include"../Model.h"
#include"Camera.h"
struct ImGui_bool
{
	bool draw_vtx_normal{ false };
	bool draw_face_normal{ false };
};

class Simple_render : public Scene
{
public:
	Simple_render();
	~Simple_render();
	void init() override;
	void Update(float deltaTime)  override;
	void Draw() override;
	void UnLoad() override;
	void OnImGuiRender() override;

private:
	Camera camera;
	std::vector<Model> models;

	Line circle;
	Model sphere;
	ImGui_bool flag;
	glm::vec3 light_pos;
	void create_spheres_and_lines();
};