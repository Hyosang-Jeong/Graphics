#include"Model.h"
#include<glm/glm.hpp>

class Object
{
public:
    Object(std::string shader);
    void Draw_vtx_normal(Model& vertex_array, glm::mat4 view, glm::mat4 projection);
    void Draw_face_normal(Model& vertex_array, glm::mat4 view, glm::mat4 projection);
    
    void Draw( Model& vertex_array, glm::mat4 view, glm::mat4 projection);
    
    void setup_shdrpgm(std::string shader);

    GLuint Shader_handle;
    glm::vec3 position{ 0,0,0 };
    glm::vec3 scale = { 1,1,1 };
    glm::vec3 rotation{ 0,0,0 };
    unsigned int model_to_use = 0;
};