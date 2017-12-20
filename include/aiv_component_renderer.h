#include <aiv_component.h>

static GLuint sprite_vao;

typedef struct aiv_component_renderer
{
    aiv_component_t component;
    GLuint vao;
    GLuint texture_id;
    GLuint program_id;
    GLuint nvertices;
} aiv_component_renderer_t;

static void _tick(aiv_component_renderer_t *self)
{
    glUseProgram(self->program_id);
    glBindVertexArray(self->vao);

    glDrawArrays(GL_DRAW_TRIANGLE, self->nvertices);
}

void aiv_component_renderer_init(aiv_component_renderer_t *self)
{
    glGenVertexArrays(1, &self->vao);
    self->component->tick = _tick;
}