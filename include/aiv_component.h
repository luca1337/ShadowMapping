#include <string.h>
#include <stdlib.h>

typedef struct aiv_component
{
    void (*begin)(aiv_component_t *);
    void (*tick)(aiv_component_t *);
    aiv_component_t *prev;
    aiv_component_t *next;
} aiv_component_t;

aiv_component_t *aiv_component_allocate_and_init(size_t len, void (*init)(aiv_component_t *));

#define aiv_new_component(t) aiv_component_allocate_and_init(sizeof(t ##_t), (void (*init)(aiv_component_t *))t ## _init)

#ifndef AIV_COMPONENT_IMPLEMENTATION
#define AIV_COMPONENT_IMPLEMENTATION 1


aiv_component_t *aiv_component_allocate_and_init(size_t len, void (*init)(aiv_component_t *))
{
    aiv_component_t *component = (aiv_component_t *)malloc(len);
    if (!component)
        return NULL;
    memset(component, 0, len);
    init(component);
    return component;
}
#endif