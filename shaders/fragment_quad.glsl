#version 330 core

in vec2 uv;
uniform sampler2D tex;
out vec4 color;

void main()
{
    float z = texture(tex, uv).r;
    if (z > 0.005)
    {
        color = vec4(z, z, z, 1);
    }
    else
    {
        color = vec4(1, 0, 0, 1);
    }
}