#version 330 core

layout(location=0) in vec2 vertex;
layout(location=1) in vec2 model_uv;

out vec2 uv;

void main()
{
    uv = model_uv;
    gl_Position = vec4(vertex, 0, 1);
}