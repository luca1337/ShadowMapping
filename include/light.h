#pragma once

#include <glad/glad.h>

typedef struct vec3
{
    float x;
    float y;
    float z;
} vec3_t;

typedef struct light
{
    vec3_t light_pos;
    vec3_t light_rot;

    GLuint light_pos_loc;
    GLuint light_rot_loc;

    float* range;
}light_t;

light_t* light_new(void);
void light_destroy(light_t* light);

void light_tick(light_t* light);