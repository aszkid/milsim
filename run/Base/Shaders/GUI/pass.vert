#version 330 core
layout (location = 0) in vec3 aPos;

void main()
{
    gl_Position = (aPos.xyz, 1.0);
}