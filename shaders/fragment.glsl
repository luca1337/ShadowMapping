#version 330 core

out vec4 color;
in vec2 uv;

// texture unit 0
uniform sampler2D tex;


in vec3 light_vertex_out;
in vec3 light_normal_out;

// texture unit 1
uniform sampler2D shadow_map;

in vec3 shadow_projection;

void main()
{
    //color = vec4(out_color, 1);
    //vec3 color1 = texture2D(tex, uv).xyz;
    //vec3 color2 = texture2D(tex2, uv).xyz;
    //color = vec4(color1, 1);

    vec3 shadow_uv = shadow_projection * 0.5 + 0.5;
    float shadow_z = shadow_uv.z;

    float shadow_value = texture(shadow_map, shadow_uv.xy).r;

    // in shadow
    if (shadow_value < shadow_z)
    {
        color = vec4(0, 0, 0, 1);
        return;
    }


    float lambert = clamp(dot(light_vertex_out, light_normal_out), 0, 1);

    color = vec4(vec3(1, 0, 0) * lambert * shadow_value, 1);
}