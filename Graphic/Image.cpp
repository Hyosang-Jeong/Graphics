/* Start Header -------------------------------------------------------
Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: Image.cpp
Purpose: Image load
Language: c++ Microsoft Visual Studio
Platform: Microsoft Visual Studio2019, Windows
Project:  Hyosang Jung_CS300_2
Author: Hyosang Jung, hyosang.jung, 055587
Creation date: 2022 - 10 - 28
End Header --------------------------------------------------------*/
#include"Image.h"
#include<iostream>
#include<fstream>
#include<string>
#include <GL/glew.h> 
#include<GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
unsigned int load_ppm(const char* path)
{
    unsigned int textureID;
    unsigned char* ppm;
    std::ifstream inFile(path);
    if (inFile.is_open() == false)
    {
        std::cerr << "Impossible to open the file !" << path;
        std::exit(EXIT_FAILURE);
    }
    std::string label;
    std::getline(inFile, label);   //P3
    std::getline(inFile, label);   //#
    unsigned width;
    unsigned height;
    unsigned max;
    inFile >> width; // width
    inFile >> height; // width
    inFile >> max; // width
    ppm = new  unsigned char[width * height * 3];
    int index = 0;

    while (inFile.eof() == false)
    {
        inFile >> label;
        unsigned char val = atoi(label.c_str());
        ppm[index++] = static_cast<unsigned char>(val);
    }

    glGenTextures(1, &textureID);

    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, ppm);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    delete[] ppm;


    return textureID;
}

unsigned int load_image(const char* path)
{
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    stbi_set_flip_vertically_on_load(true);
    // set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    // load and generate the texture
    int width, height, nrChannels;
    unsigned char* data = stbi_load(path, &width, &height, &nrChannels, 0);
    if (data)
    {
        if (nrChannels == 3)
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        }
        else
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        }
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    return texture;
}



