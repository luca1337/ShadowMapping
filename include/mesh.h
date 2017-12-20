#include <glad/glad.h>

typedef struct v3
{
    float x;
    float y;
    float z;
} v3_t;

typedef struct mesh
{
    v3_t position;
    v3_t rotation;
    v3_t scale;

    v3_t camera_position;
    v3_t camera_rotation;

    v3_t light_rotation;

    GLuint vao;
    unsigned int faces_cnt;
    float use_ortho;
} mesh_t;

typedef struct shader_context
{
    GLuint program;

    GLint position;
    GLint rotation;
    GLint scale;

    GLint camera_position;
    GLint camera_rotation;
    
    GLint light_rotation;
    

    GLint use_ortho;
} shader_context_t;

int mesh_new(mesh_t *, char *);
void mesh_draw(mesh_t *, shader_context_t);