#version 330 core


layout(location=0) in vec2 vertex;
layout(location=1) in vec2 uv;

out vec2 uv_out;

void main()
{
    uv_out = uv;
    gl_Position = vec4(vertex/2, 0, 1);
}