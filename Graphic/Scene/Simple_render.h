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
	Sphere,
	count
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
	void create_spheres_and_lines();
	void create_objects();
	Camera camera;

	Model models[count];
	std::vector<Object> objects;

	Line circle;
	glm::vec3 light_pos;
	ImGui_bool flag;
	int model_to_draw{ 0 };
};