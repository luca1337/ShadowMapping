 #include <light.h>
 #include <stdio.h>

 light_t* light_new(void)
 {
    light_t* light = malloc(sizeof(light_t));

    if(!light)
        return (void*)0;

    light->light_pos.x = 0.0f;
    light->light_pos.y = 0.0f;
    light->light_pos.z = 0.0f;

    light->light_rot.x = 0.0f;
    light->light_rot.y = 0.0f;
    light->light_rot.z = 0.0f;

    light->range = malloc(sizeof(float*) * 10000);
    return light;
 }

 void light_tick(light_t* light)
 {
     glUniform3f(light->light_pos_loc, light->light_pos.x, light->light_pos.y, light->light_pos.z);
     glUniform3f(light->light_rot_loc, light->light_rot.x, light->light_rot.y, light->light_rot.z);
 }

void light_destroy(light_t* light)
{
    if(!light)
        fprintf(stderr, "couldn not free memory\nprobably no mem allocated for that buffer");

    free(light->range);
    free(light); 
}