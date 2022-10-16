/* Start Header -------------------------------------------------------
Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: main.cpp
Purpose: main function to run the program
Language: c++ Microsoft Visual Studio
Platform: Microsoft Visual Studio2019, Windows
Project:  Hyosang Jung_CS300_1
Author: Hyosang Jung, hyosang.jung, 055587
Creation date: 2022 - 09 - 10
End Header --------------------------------------------------------*/
#include"Engine.h"
#include"Scene/Simple_render.h"
int main()
{
	Engine engine(1600,1000);
    Simple_render* simple_render = new Simple_render();
    engine.Add(simple_render);
    while (!engine.ShouldClose())
    {
        engine.Update();
        engine.Draw();
    }
    return 0;
}