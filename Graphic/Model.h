#pragma once
#include <vector>
#include <GL/glew.h> 
#include<glm/glm.hpp>
#include<string>
#include"shader.h"


struct Vertex
{
    Vertex(glm::vec3 pos, glm::vec3 nrm) : pos(pos), vtx_nrm(nrm)  {}

    Vertex() { }

    glm::vec3 pos{ 0 };
    glm::vec3 vtx_nrm{ 0 };
    glm::vec3 color{ 0 };
};

constexpr int vertexSize = sizeof(Vertex);
constexpr int indexSize = sizeof(int);

struct Face
{
    Face(int index0, int index1, int index2, glm::vec3 normal)
    {
        index[0] = index0;
        index[1] = index1;
        index[2] = index2;
        nrm = normal;
    }
    int index[3];
    glm::vec3 nrm{ 0 };
};

/*  The layouts for specifying the offsets of a vertex
    when it is copied into the graphics pipeline. */
struct VertexLayout
{
    int location;
    int size;
    int type;
    bool normalized;
    int offset;
};

const VertexLayout vLayout[] =
{
    { 0, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, pos) },
    { 1, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, vtx_nrm) },
    {2, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, color) }
};

constexpr int layoutSize = sizeof(VertexLayout);
constexpr int numAttribs = sizeof(vLayout) / layoutSize;    // for now numAttribs = 1: only pos

class Line
{
public:
    Line() :VAO(0), VBO(0), Shader_handle(0)
    {}
    void init(std::vector<glm::vec3> points);
    void send_data();
    void Draw(glm::mat4 view, glm::mat4 projection, glm::mat4 model = { 1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1 });
    GLuint VAO;
    GLuint VBO;
    GLuint Shader_handle;
    std::vector<glm::vec3> vertices;
};

/*  Mesh format */
class Model
{
public:
    Model() : numVertices(0), numTris(0), numIndices(0), vertex_per_face{ 3 },position { 0 }, scale{ 1 }, rotation{ 0 }, VBO{ 0 }, VAO{ 0 }, IBO{ 0 }, Shader_handle{ 0 }
    { }

    std::vector<Vertex> vertexBuffer;
    std::vector<int> indexBuffer;
    std::vector<Face> FaceBuffer;
    int numVertices;
    int numTris;
    int numIndices;

    int vertex_per_face; // number of vertex per face

    GLuint VAO;
    GLuint VBO;
    GLuint IBO;
    GLuint Shader_handle;
    Line vertex_normal;
    Line face_normal;

    glm::mat4 model =
    {
     1,0,0,0,
     0,1,0,0,
     0,0,1,0,
     0,0,0,1
    };
    void init(std::string shader, glm::vec3 Pos = { 0,0,0 }, glm::vec3 Scale = { 1,1,1 }, glm::vec3 Rotate = { 0,0,0 });
    void addVertex(const Vertex& v);
    void addIndex( int index);
    void SendVertexData();
    bool DegenerateTri(const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2);
    void BuildIndexBuffer(int stacks, int slices);
    void setup_shdrpgm(std::string shader);

    void Draw(glm::mat4 view, glm::mat4 projection);
    void Draw_vtx_normal(glm::mat4 view, glm::mat4 projection);
    void Draw_face_normal(glm::mat4 view, glm::mat4 projection);

    void Calculate_normal();
    void Set_vertex_normal();
    void Set_face_normal();

    void set_position(glm::vec3 pos)
    {
        position = pos;
    }
    void set_rotation(glm::vec3 angle)
    {
        rotation = angle;
    }
    void set_scale(float factor)
    {
        scale *= factor;
    }
    glm::vec3& Get_position()
    {
        return position;
    }
    glm::vec3& Get_Rotation()
    {
        return rotation;
    }
    glm::vec3& Get_Scale()
    {
        return scale;
    }
private:
    glm::vec3 position{ 0,0,0 };
    glm::vec3 scale = { 1,1,1 };
    glm::vec3 rotation{ 0,0,0 };
};

Model create_sphere(int stacks, int sector);

Model load_obj(const char* path);



