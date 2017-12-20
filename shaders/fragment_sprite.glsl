#version 330 core

out vec4 color;
in vec2 uv_out;
uniform sampler2D tex;


void main()
{
    vec3 color1 = texture(tex, uv_out).xyz;
    //color1 = vec3(1, 0 ,0);
    color = vec4(color1 * vec3(1, 1, 0), 1);
}