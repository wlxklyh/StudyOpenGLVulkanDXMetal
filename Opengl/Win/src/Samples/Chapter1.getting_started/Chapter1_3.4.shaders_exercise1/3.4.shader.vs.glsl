#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

out vec3 ourColor;

void main()
{
    //-aPos.y!!!
    gl_Position = vec4(aPos.x, -aPos.y, aPos.z, 1.0);
    ourColor = vec3(aColor.y,aColor.x,aColor.z);
}