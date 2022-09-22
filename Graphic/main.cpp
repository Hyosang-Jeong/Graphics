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