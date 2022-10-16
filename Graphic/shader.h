/* Start Header -------------------------------------------------------
Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: shader.h
Purpose: Declare shader functions
Language: c++ Microsoft Visual Studio
Platform: Microsoft Visual Studio2019, Windows
Project:  Hyosang Jung_CS300_1
Author: Hyosang Jung, hyosang.jung, 055587. code from digipen
Creation date: 2022 - 09 - 14
End Header --------------------------------------------------------*/
#pragma once

#include<gl/glew.h>
enum Primitive_Enum
{
    TriangleStrip = 0,
    Points,
    LineStrip
};

///////////////////
GLuint LoadShaders(const char * vertex_file_path,const char * fragment_file_path);

///////////////////
GLuint LoadPipeline(const char * vertex_file_path,const char * fragment_file_path, GLuint * programIDs );

// Load shaders where multiple shader files == one complete shader
// Show how code can be reused across shaders
GLuint LoadMultiShaders(const char *vertex_file_path, const char *fragment_file_path,
                        const char *geom_file_path, Primitive_Enum  out_primitive_type );

