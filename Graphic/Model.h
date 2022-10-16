/* Start Header -------------------------------------------------------
Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: Model.h
Purpose: Declare Vertex, Model, Line class and functions.
Language: c++ Microsoft Visual Studio
Platform: Microsoft Visual Studio2019, Windows
Project:  Hyosang Jung_CS300_1
Author: Hyosang Jung, hyosang.jung, 055587
Creation date: 2022 - 09 - 12
End Header --------------------------------------------------------*/
#pragma once
#include <vector>
#include <GL/glew.h> 
#include<glm/glm.hpp>
#include<string>
#include"shader.h"
#include<map>


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
    Model() : numVertices(0), numTris(0), numIndices(0), vertex_per_face{ 3 }, VBO{ 0 }, VAO{ 0 }, IBO{ 0 }
    { }
    ~Model();
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
    Line vertex_normal;
    Line face_normal;

    void addVertex(const Vertex& v);
    void addIndex( int index);
    void SendVertexData();
    bool DegenerateTri(const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2);
    void BuildIndexBuffer(int stacks, int slices);

    void Calculate_normal();
    void Set_vertex_normal();
    void Set_face_normal();

    void Use();
};

Model* create_sphere(int stacks, int sector);
Model* load_obj(const char* path);



