#define _CRT_SECURE_NO_WARNINGS
#include"Model.h"
#include <glm/gtc/matrix_transform.hpp> //translate, rotate, scale, perspective 

#include<glm/gtc/constants.hpp>
#include<map>
#include<algorithm>
#include<fstream>
#include<iostream>
void Model::init(std::string shader, glm::vec3 , glm::vec3 , glm::vec3 Rotate)
{
   // position = Pos;
   // scale = Scale;
    rotation = Rotate;
    setup_shdrpgm(shader);
  //  SendVertexData();
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

void Model::setup_shdrpgm(std::string shader)
{
    std::string vert = "../shaders/";
    std::string frag = "../shaders/";
    vert = vert + shader + ".vert";
    frag = frag + shader + ".frag";

    Shader_handle = LoadShaders(vert.c_str(), frag.c_str());
}

void Model::Draw( glm::mat4 view, glm::mat4 projection)
{
    glBindVertexArray(VAO);
   // glBindBuffer(GL_ARRAY_BUFFER, VBO);
    GLint model_loc = glGetUniformLocation(Shader_handle, "model");
    GLint view_loc = glGetUniformLocation(Shader_handle, "view");
    GLint projection_loc = glGetUniformLocation(Shader_handle, "projection");

    model = glm::translate(model, position);
    model = glm::rotate(model, rotation.x, { 1,0,0 });
    model = glm::rotate(model, rotation.y, { 0,1,0 });
    model = glm::rotate(model, rotation.z, { 0,0,1 });
    model = glm::scale(model, { scale.x,scale.y,scale.z });

    glUniformMatrix4fv(model_loc, 1, GL_FALSE, &model[0].x);
    glUniformMatrix4fv(view_loc, 1, GL_FALSE, &view[0].x);
    glUniformMatrix4fv(projection_loc, 1, GL_FALSE, &projection[0].x);
    glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, nullptr);
    model = {
     1,0,0,0,
     0,1,0,0,
     0,0,1,0,
     0,0,0,1
    };
}

void Model::Draw_vtx_normal(glm::mat4 view, glm::mat4 projection)
{
    model = glm::translate(model, position);
    model = glm::rotate(model, rotation.x, { 1,0,0 });
    model = glm::rotate(model, rotation.y, { 0,1,0 });
    model = glm::rotate(model, rotation.z, { 0,0,1 });
    model = glm::scale(model, { scale.x,scale.y,scale.z });
    vertex_normal.Draw(view, projection, model);
    model = {
        1,0,0,0,
        0,1,0,0,
        0,0,1,0,
        0,0,0,1
    };
}
void Model::Draw_face_normal(glm::mat4 view, glm::mat4 projection)
{
    model = glm::translate(model, position);
    model = glm::rotate(model, rotation.x, { 1,0,0 });
    model = glm::rotate(model, rotation.y, { 0,1,0 });
    model = glm::rotate(model, rotation.z, { 0,0,1 });
    model = glm::scale(model, { scale.x,scale.y,scale.z });
    face_normal.Draw(view, projection, model);
    model = {
        1,0,0,0,
        0,1,0,0,
        0,0,1,0,
        0,0,0,1
    };
}

void Model::Calculate_normal()
{
    std::map<int, std::vector<glm::vec3>> accumulation;

    for (int i = 0; i < numIndices; i+=vertex_per_face)
    {
        int index1 = indexBuffer[i];
        int index2 = indexBuffer[i+1];
        int index3 = indexBuffer[i+2];
        
        glm::vec3 vec1 = vertexBuffer[index2].pos - vertexBuffer[index1].pos;
        glm::vec3 vec2 = vertexBuffer[index3].pos - vertexBuffer[index1].pos;
        glm::vec3 vec3 = glm::cross(vec1, vec2);

        FaceBuffer.push_back({ indexBuffer[i] , indexBuffer[i + 1] , indexBuffer[i + 2] ,glm::normalize(vec3) });

        accumulation[indexBuffer[i]].push_back(glm::normalize(vec3));
        accumulation[indexBuffer[i+1]].push_back(glm::normalize(vec3));
        accumulation[indexBuffer[i+2]].push_back(glm::normalize(vec3));
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
void Model::Set_vertex_normal()
{
    std::vector<glm::vec3> vertices;
    for (int i = 0; i < vertexBuffer.size(); i++)
    {
        vertices.push_back(vertexBuffer[i].pos);
        vertexBuffer[i].vtx_nrm = glm::normalize(vertexBuffer[i].vtx_nrm);
        vertices.push_back(vertexBuffer[i].pos + (vertexBuffer[i].vtx_nrm / (scale * 8.f)));
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
        vertices.push_back(pos + (FaceBuffer[i].nrm / (scale * 8.f)));
    }
    face_normal.init(vertices);
}
Model create_sphere(int stacks, int slices)
{
    Model mesh;
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
            mesh. addVertex( v);
        }
    }

    mesh.BuildIndexBuffer(stacks, slices);
    mesh.Calculate_normal();
    mesh.SendVertexData();

    return mesh;
}

Model load_obj(const char* path)
{
    Model model;
    std::vector< unsigned int > vertexIndices;
    std::vector< glm::vec3 > temp_vertices;
    std::ifstream inFile(path);
    if (inFile.is_open() == false)
    {
        std::cerr << "Impossible to open the file !";
        std::exit(EXIT_FAILURE);
    }
    float min_x = 100.f;
    float min_y = 100.f;
    float min_z = 100.f;
    float max_x = -100.f;
    float max_y = -100.f;
    float max_z = -100.f;
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

            if (vertex.x < min_x)
            {
                min_x = vertex.x;
            }
            if (vertex.y < min_y)
            {
                min_y = vertex.y;
            }
            if (vertex.z < min_z)
            {
                min_z = vertex.z;
            }
            if (vertex.x > max_x)
            {
                max_x = vertex.x;
            }
            if (vertex.y > max_y)
            {
                max_y = vertex.y;
            }
            if (vertex.z > max_z)
            {
                max_z = vertex.z;
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
           // model.vertex_per_face = static_cast<int>(tmp_index.size());
        }

    }
    for (auto& i : tmp_index)
    {
        vertexIndices.push_back(i);
    }
    glm::vec3 center{ 0,0,0 };
    for (unsigned int i = 0; i < temp_vertices.size(); i++)
    {
        Vertex v;
        v.pos = temp_vertices[i];
        center += temp_vertices[i];
        model.addVertex(v);
    }
    center /= static_cast<float>(temp_vertices.size());

    for (unsigned int i = 0; i < vertexIndices.size(); i++)
    {
        model.addIndex(vertexIndices[i] - 1);
    }
    for (unsigned int i = 0; i < vertexIndices.size(); i++)
    {
        model.addIndex(vertexIndices[i] - 1);
    }

    for (auto& vertex : model.vertexBuffer)
    {
        vertex.pos -= center;
        vertex.pos.x *= 2.f / (max_x - min_x);
        vertex.pos.y *= 2.f / (max_y - min_y);
        vertex.pos.z *= 2.f / (max_z - min_z) ;
    }
    model.Calculate_normal();
    model.SendVertexData();
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
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * vertices.size() , &vertices[0].x, GL_STATIC_DRAW);
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
