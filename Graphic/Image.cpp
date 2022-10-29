#include"Image.h"
#include<iostream>
#include<fstream>
#include<string>
#include <GL/glew.h> 
#include<GLFW/glfw3.h>
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

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    delete[] ppm;


    return textureID;
}