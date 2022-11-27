/* Start Header -------------------------------------------------------
Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: Model.cpp
Purpose: Implement model related functions. Load obj file. Create models.
Language: c++ Microsoft Visual Studio
Platform: Microsoft Visual Studio2019, Windows
Project:  Hyosang Jung_CS300_1
Author: Hyosang Jung, hyosang.jung, 055587
Creation date: 2022 - 09 - 12
End Header --------------------------------------------------------*/
#include"Model.h"
#include <glm/gtc/matrix_transform.hpp> //translate, rotate, scale, perspective 
#include<glm/gtc/constants.hpp>
#include<map>
#include<algorithm>
#include<fstream>
#include<iostream>


Model::~Model()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &IBO);
}

void Model::addVertex(const Vertex& v)
{
    vertexBuffer.push_back(v);
    ++numVertices;
}

void Model::addIndex(int index)
{
   indexBuffer.push_back(index);
    ++numIndices;

    if (numIndices % 3 == 0)
        ++numTris;
}

void Model::SendVertexData()
{
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    /*  Copy vertex attributes to GPU */
    glBufferData(GL_ARRAY_BUFFER,
        static_cast<GLsizeiptr>(numVertices) * static_cast<GLsizeiptr>(vertexSize), &vertexBuffer[0],
        GL_DYNAMIC_DRAW);

    glGenBuffers(1, &IBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
    /*  Copy vertex indices to GPU */
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
        static_cast<GLsizeiptr>(numIndices) * static_cast<GLsizeiptr>(indexSize), &indexBuffer[0],
        GL_DYNAMIC_DRAW);

    /*  Send vertex attributes to shaders */
    for (int i = 0; i < numAttribs; ++i)
    {
        glEnableVertexAttribArray(vLayout[i].location);
        glVertexAttribPointer(vLayout[i].location, vLayout[i].size, vLayout[i].type, vLayout[i].normalized, vertexSize, (void*)(uintptr_t)vLayout[i].offset);
    }
}

bool Model::DegenerateTri(const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2)
{
    return (glm::distance(v0, v1) < 0.00001f ||
        glm::distance(v1, v2) < 0.00001f ||
        glm::distance(v2, v0) < 0.00001f);
}
void Model::BuildIndexBuffer(int stacks, int slices)
{
    int p0 = 0, p1 = 0, p2 = 0;
    int p3 = 0, p4 = 0, p5 = 0;

    int stride = slices + 1;
    for (int i = 0; i < stacks; ++i)
    {
        for (int j = 0; j < slices; ++j)
        {
            int current_row = i * stride;
            p0 = current_row + j;
            p1 = p0 + 1;
            p2 = p1 + stride;
            if (!DegenerateTri(vertexBuffer[p0].pos,
              vertexBuffer[p1].pos,
                vertexBuffer[p2].pos))
            {
                addIndex( p0);
                addIndex(p1);
                addIndex( p2);
            }
            p3 = p2;
            p4 = p3 - 1;
            p5 = p0;
            if (!DegenerateTri(vertexBuffer[p3].pos,
                vertexBuffer[p4].pos,
                vertexBuffer[p5].pos))
            {
                addIndex( p3);
                addIndex( p4);
                addIndex(p5);
            }
        }
    }
}
void Model::Calculate_normal()
{
    std::map<int, std::vector<glm::vec3>> accumulation;

    for (int i = 0; i < numIndices; i+=vertex_per_face)
    {
        int index1 =*(indexBuffer.begin() + i);
        int index2 = *(indexBuffer.begin() + i + 1);
        int index3 = * (indexBuffer.begin() + i + 2);
        
        glm::vec3 vec1 = vertexBuffer[index2].pos - vertexBuffer[index1].pos;
        glm::vec3 vec2 = vertexBuffer[index3].pos - vertexBuffer[index1].pos;
        glm::vec3 vec3 = glm::cross(vec1, vec2);

        FaceBuffer.push_back({ indexBuffer[i] , *(indexBuffer.begin() + i + 1), *(indexBuffer.begin() + i + 2) ,glm::normalize(vec3) });

        accumulation[index1].push_back(glm::normalize(vec3));
        accumulation[index2].push_back(glm::normalize(vec3));
        accumulation[index3].push_back(glm::normalize(vec3));
    }

    for (auto& i : accumulation)
    {
        i.second.erase(std::unique(i.second.begin(), i.second.end()), i.second.end());
        for (auto& j : i.second)
        {
            vertexBuffer[i.first].vtx_nrm += glm::normalize(j);
        }
    }

    Set_vertex_normal();
    Set_face_normal();
}
void Model::Calculate_uv_planar(bool usePos)
{
    for (auto& vertex : vertexBuffer)
    {
        glm::vec3 absVec;
        glm::vec3 v;
        if (usePos)
        {
            absVec = abs(vertex.pos);
            v = vertex.pos;
        }
        else
        {
            absVec = abs(vertex.vtx_nrm);
            v = (glm::normalize(vertex.vtx_nrm) + 1.f)/2.f;
        }
        glm::vec2 uv = glm::vec2(0.0);

        if (absVec.x >= absVec.y && absVec.x >= absVec.z)
        {
            (v.x < 0.0) ? (uv.x =-v.z/absVec.x) : (uv.x = v.z / absVec.x);
            uv.y = v.y;
        }
        else if (absVec.y >= absVec.x && absVec.y >= absVec.z)
        {
            (v.y < 0.0) ? (uv.x =-v.x / absVec.y) : (uv.x = v.x / absVec.y);
            uv.y = v.z / absVec.y;
        }
        else if (absVec.z >= absVec.y && absVec.z >= absVec.x)
        {
            (v.z < 0.0) ? (uv.x = -v.x / absVec.z) : (uv.x = v.x / absVec.z);
            uv.y = v.y / absVec.z;
        }
        vertex.uv = (uv+1.f)/2.f;
    }
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    /*  Copy vertex attributes to GPU */
    glBufferData(GL_ARRAY_BUFFER,
        static_cast<GLsizeiptr>(numVertices) * static_cast<GLsizeiptr>(vertexSize), &vertexBuffer[0],
        GL_DYNAMIC_DRAW);
}
void Model::Calculate_uv_cylindrical(bool usePos)
{

    for (auto& vertex : vertexBuffer)
    {
        glm::vec3 v;
        if (usePos)
            v = vertex.pos;
        else
            v = vertex.vtx_nrm;
        float ux = glm::degrees(atan2(v.z, v.x));
        ux += 180.f;
        float uy = (v.y+1.f) / (2.f);
        vertex.uv.x = ux / 360.f;
        vertex.uv.y = uy;
    }
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    /*  Copy vertex attributes to GPU */
    glBufferData(GL_ARRAY_BUFFER,
        static_cast<GLsizeiptr>(numVertices) * static_cast<GLsizeiptr>(vertexSize), &vertexBuffer[0],
        GL_DYNAMIC_DRAW);
}

void Model::Calculate_uv_spherical(bool usePos)
{
    for (auto& vertex : vertexBuffer)
    {
        glm::vec3 v;
        if (usePos)
            v = vertex.pos;
        else
            v = vertex.vtx_nrm;
        float  r = sqrt((v.x * v.x) + (v.y * v.y) + (v.z * v.z));
        float theta = glm::degrees(atan2(v.z, v.x));
        theta += 180.f;
        float phi = 180.f - glm::degrees(std::acos(v.y / r));
        
        vertex.uv.x = theta / 360.f;
        vertex.uv.y = phi / 180.f;
    }

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    /*  Copy vertex attributes to GPU */
    glBufferData(GL_ARRAY_BUFFER,
        static_cast<GLsizeiptr>(numVertices) * static_cast<GLsizeiptr>(vertexSize), &vertexBuffer[0],
        GL_DYNAMIC_DRAW);
}




void Model::Set_vertex_normal()
{
    std::vector<glm::vec3> vertices;
    for (int i = 0; i < vertexBuffer.size(); i++)
    {
        vertices.push_back(vertexBuffer[i].pos);
        vertexBuffer[i].vtx_nrm = glm::normalize(vertexBuffer[i].vtx_nrm);
        vertices.push_back(vertexBuffer[i].pos + (vertexBuffer[i].vtx_nrm / ( 8.f)));
    }
    vertex_normal.init(vertices);
}
void Model::Set_face_normal()
{
    std::vector<glm::vec3> vertices;
    for (int i = 0; i < FaceBuffer.size(); i++)
    {
        glm::vec3 pos_0 = vertexBuffer[FaceBuffer[i].index[0]].pos;
        glm::vec3 pos_1 = vertexBuffer[FaceBuffer[i].index[1]].pos;
        glm::vec3 pos_2 = vertexBuffer[FaceBuffer[i].index[2]].pos;
        glm::vec3 pos = (pos_0 + pos_1 + pos_2) / 3.f;
        vertices.push_back(pos);
        vertices.push_back(pos + (FaceBuffer[i].nrm / ( 8.f)));
    }
    face_normal.init(vertices);
}
void Model::Use()
{
    glBindVertexArray(VAO);
}
Model* create_sphere(int stacks, int slices)
{
    Model* mesh = new Model();
   constexpr float PI = glm::pi<float>();
    for (int stack = 0; stack <= stacks; ++stack)
    {
        float row = (float)stack / stacks;
        float beta = PI * (row - 0.5f);

        for (int slice = 0; slice <= slices; ++slice)
        {
            float col = (float)slice / slices;
            float alpha = col * PI * 2.0f;
            Vertex v;
            v.pos.x = 0.5f * sin(alpha) * cos(beta);
            v.pos.y = 0.5f * sin(beta);
            v.pos.z = 0.5f * cos(alpha) * cos(beta);
            mesh->addVertex( v);
        }
    }

    mesh->BuildIndexBuffer(stacks, slices);
    mesh->Calculate_normal();
    mesh->Calculate_uv_cylindrical(true);
    mesh->SendVertexData();

    return mesh;
}

Model* create_plane(int stacks, int slices)
{
    Model* model = new Model();
    for (int stack = 0; stack <= stacks; ++stack)
    {
        float row = (float)stack / stacks;

        for (int slice = 0; slice <= slices; ++slice)
        {
            float col = (float)slice / slices;

            Vertex v;

            v.pos = glm::vec3(col - 0.5f, 0.0f, 0.5f - row );

            model->addVertex(v);
        }
    }

    model->BuildIndexBuffer(stacks, slices);
    model->Calculate_normal();
    model->SendVertexData();
    return model;
}

Model* load_obj(const char* path)
{
    Model* model = new Model();
    std::vector< unsigned int > vertexIndices;
    std::vector< glm::vec3 > temp_vertices;
    std::ifstream inFile(path);
    if (inFile.is_open() == false)
    {
        std::cerr << "Impossible to open the file !";
        std::exit(EXIT_FAILURE);
    }
    glm::vec3 min{100.f};
    glm::vec3 max{-100.f};
    std::string label;
    std::vector<unsigned int> tmp_index;
    while (inFile.eof() == false)
    {
        inFile >> label;
       
        if (label== "v")
        {
            glm::vec3 vertex;
            inFile >> vertex.x;
            inFile >> vertex.y;
            inFile >> vertex.z;

            if (vertex.x < min.x)
            {
                min.x = vertex.x;
            }
            if (vertex.y < min.y)
            {
                min.y = vertex.y;
            }
            if (vertex.z < min.z)
            {
                min.z = vertex.z;
            }
            if (vertex.x > max.x)
            {
                max.x = vertex.x;
            }
            if (vertex.y > max.y)
            {
                max.y = vertex.y;
            }
            if (vertex.z > max.z)
            {
                max.z = vertex.z;
            }
            temp_vertices.push_back(vertex);
        }

        else if (label == "f")
        {
            int index = 0;
            inFile >> index;
             if (index == 0)
             {
                 break;
             }
             tmp_index.push_back(index);
             inFile >> index;
             tmp_index.push_back(index);          
             inFile >> index;
             tmp_index.push_back(index);
        }

    }
    for (auto& i : tmp_index)
    {
        vertexIndices.push_back(i);
    }
    for (unsigned int i = 0; i < temp_vertices.size(); i++)
    {
        Vertex v;
        v.pos = temp_vertices[i];
        model->addVertex(v);
    }
    for (unsigned int i = 0; i < vertexIndices.size(); i++)
    {
        model->addIndex(vertexIndices[i] - 1);
    }

    for (auto& vertex : model->vertexBuffer)
    {
        vertex.pos.x = max.x == min.x? 0 :  (2.f * ((vertex.pos.x - min.x) / (max.x - min.x))) - 1.f;
        vertex.pos.y = max.y == min.y ? 0 : (2.f * ((vertex.pos.y - min.y) / (max.y - min.y))) - 1.f;
        vertex.pos.z = max.z == min.z ? 0 :  (2.f * ((vertex.pos.z - min.z) / (max.z - min.z))) - 1.f;
    }
    model->Calculate_normal();
    model->Calculate_uv_spherical(true);
    model->SendVertexData();
    return model;
}

Model* create_cubeMap()
{
    Model* model = new Model();

    glm::vec3 skyboxVertices[] = {
        // positions      
        //front face
          glm::vec3 {- 1.0f,  -1.0f, 1.0f },
          glm::vec3 {-1.0f, 1.0f, 1.0f},
          glm::vec3 {1.0f, 1.0f,  1.0f},
          glm::vec3 {1.0f, -1.0f, 1.0f},

         //back face
        glm::vec3 {-1.0f,  -1.0f, -1.0f},
        glm::vec3 {-1.0f, 1.0f,   -1.0f},
        glm::vec3 { 1.0f, 1.0f,    -1.0f},
        glm::vec3 { 1.0f, -1.0f,  -1.0f},
    };

    for (int i = 0; i < 8; i++)
    {
        Vertex v;
        v.pos = skyboxVertices[i];
        model->addVertex(v);
    }

    //front
    model->addIndex(0);
    model->addIndex(1);
    model->addIndex(2);
    model->addIndex(2);
    model->addIndex(3);
    model->addIndex(0);
     
    //back
    model->addIndex(4);
    model->addIndex(7);
    model->addIndex(6);
    model->addIndex(4);
    model->addIndex(6);
    model->addIndex(5);

    //up
    model->addIndex(1);
    model->addIndex(5);
    model->addIndex(6);
    model->addIndex(1);
    model->addIndex(6);
    model->addIndex(2);

    //down
    model->addIndex(0);
    model->addIndex(3);
    model->addIndex(7);
    model->addIndex(0);
    model->addIndex(7);
    model->addIndex(4);

    //left
    model->addIndex(0);
    model->addIndex(4);
    model->addIndex(5);
    model->addIndex(0);
    model->addIndex(5);
    model->addIndex(1);

    //right
    model->addIndex(3);
    model->addIndex(2);
    model->addIndex(6);
    model->addIndex(3);
    model->addIndex(6);
    model->addIndex(7);

    model->Calculate_uv_planar(true);
    model->SendVertexData();
    return model;
}

void Line::init(std::vector<glm::vec3> points)
{
    vertices = std::move(points);

    std::string vert = "../shaders/";
    std::string frag = "../shaders/";
    vert = vert + "line" + ".vert";
    frag = frag + "line" + ".frag";
    Shader_handle = LoadShaders(vert.c_str(), frag.c_str());
    send_data();

}

void Line::send_data()
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * vertices.size() , &vertices[0].x, GL_DYNAMIC_DRAW);
    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
}

void Line::Draw(glm::mat4 view, glm::mat4 projection, glm::mat4 model )
{
    glUseProgram(Shader_handle);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    GLint model_loc = glGetUniformLocation(Shader_handle, "model");
    GLint view_loc = glGetUniformLocation(Shader_handle, "view");
    GLint projection_loc = glGetUniformLocation(Shader_handle, "projection");

    glUniformMatrix4fv(model_loc, 1, GL_FALSE, &model[0].x);
    glUniformMatrix4fv(view_loc, 1, GL_FALSE, &view[0].x);
    glUniformMatrix4fv(projection_loc, 1, GL_FALSE, &projection[0].x);
    glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(vertices.size()));
}
