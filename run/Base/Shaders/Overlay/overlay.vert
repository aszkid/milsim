#version 330 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 texcoord;

void main()
{
    gl_Position = vec4(aPos, 1.0);
}