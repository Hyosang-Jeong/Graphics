Name: Hyosang Jung
Digipen Id: hyosang.jung

There is a ImGui that can control following features.
1. Check button to draw vertex normal.
2. Check button to draw face normal.
3. Select central object.
4. Change light position.
5. Change camera position.
6. Set camera's yaw and pitch.

Any assumption that you make on how to use the application that, if violated, might cause
the application to fail.
None

Which part of the assignment has been completed?
1.Read obj file and load meshes. - Graphic/model.h,model.cpp - load_obj function

2.Generate spheres. - Graphic/model.h,model.cpp-create_sphere function

3.Draw each meshes - Central object and rotating spheres -Graphic/Scene/Simple_render.cpp - Draw function

4.Calculate vertex and face normals. - Graphic/model.h,model.cpp - Calculate_normal function

5.Draw vertex and face normals. - Graphic/model.h,model.cpp - Line::Draw function

6.Implementing vertex and fragment shaders corresponding to Phong Lighting - shaders/simple_render.frag file

7.Add ImGUI control -Graphic/Scene/Simple_render.cpp - OnImGuiRender function

Which part of the assignment has NOT been completed (not done, not working, etc.) and
explanation on why those parts are not completed?
None

Where the relevant source codes (both C++ and shaders) for the assignment are located.
Specify the file path (folder name), file name, function name (or line number).


Which machine did you test your application on.
Remote
Windows 11 Pro
OpenGL Extensions Viewer to get this information readily.
NVIDIA GeForce GTX 1650 with Max-Q Design/PCIe/SSE2
GL_VERSION: 4.6.0 NVIDIA 516.59
GL_SHADING_LANGUAGE_VERSION: 4.60 NVIDIA


i. The average number of hours you spent on the assignment, on a weekly basis
Total 10 hours
weekly 3 hours

j. Any other useful information pertaining to the application 
No.