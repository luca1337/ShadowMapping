#version 330 core

layout(location=0) in vec3 vertex;
layout(location=2) in vec3 normal;
layout(location=1) in vec2 model_uv;

const float PI = 3.1415926535;

out vec2 uv;

uniform vec3 position;
uniform vec3 rotation;
uniform vec3 scale;

uniform vec3 camera_position;
uniform vec3 camera_rotation;

uniform vec3 light_rot;
uniform vec3 light_pos;


out vec3 light_vertex_out;
out vec3 light_normal_out;

uniform float use_ortho;

out vec3 shadow_projection;

vec3 rotate_y(vec3 v, float angle)
{
    angle = radians(angle);
    vec3 v0 = v;
    v.x = v0.x * cos(angle) + v0.z * sin(angle);
    v.z = v0.x * -sin(angle) + v0.z * cos(angle);
    return v;
}

vec3 rotate_x(vec3 v, float angle)
{
    angle = radians(angle);
    vec3 v0 = v;
    v.y = v0.y * cos(angle) + v0.z * -sin(angle);
    v.z = v0.y * sin(angle) + v0.z * cos(angle);
    return v;
}

vec3 rotate_z(vec3 v, float angle)
{
    angle = radians(angle);
    vec3 v0 = v;
    v.x = v0.x * cos(angle) + v0.y * -sin(angle);
    v.y = v0.x * sin(angle) + v0.y * cos(angle);
    return v;
}

vec3 rotate(vec3 v, vec3 r)
{
    // yzx
    v = rotate_y(v, r.y);
    v = rotate_z(v, r.z);
    v = rotate_x(v, r.x);
    return v;
}

vec3 compute_perspective(vec3 v)
{
    v.x /= (-v.z * tan(60 * 0.5 * (PI/180)));
    v.y /= (-v.z * tan(60 * 0.5 * (PI/180)));
    v.z =  ((-v.z - 1)/(1000-1)) * 2 -1;
    return v;
}

vec3 compute_ortho(vec3 v)
{
    float limit = 20;
    v.x = ((v.x +limit)/(limit+limit)) * 2 -1;
    v.y = ((v.y + limit)/(limit+limit)) * 2 -1;
    v.z =  ((-v.z - 1)/(1000-1)) * 2 -1;
    return v;
}

void main()
{
    vec3 light = vec3(0, 0, 0);

    uv = model_uv;

    // yzx
    vec3 worldPosition = (rotate(vertex, rotation) + position) * scale;

    vec3 worldNormal = normalize(rotate(rotate(normal, rotation), -camera_rotation));

    vec3 worldView = rotate(worldPosition, -camera_rotation) - camera_position;

    vec3 vertexToLight = normalize(light - worldView);
    light_vertex_out = vertexToLight;
    light_normal_out = worldNormal;

    if (use_ortho > 0.1)
    {
        gl_Position = vec4(compute_ortho(worldView), 1);
    }
    else
    {
        gl_Position = vec4(compute_perspective(worldView), 1);
    }

    shadow_projection = compute_ortho(rotate(worldPosition, light_rot) - light_pos);
}