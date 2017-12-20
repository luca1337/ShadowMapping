#include <aiv_opengl.h>
#include <stdio.h>
#include <stdlib.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <mesh.h>
#include <light.h>

GLuint aiv_compile_shader(char *, char *);

aiv_opengl_context_t context;


int main(int argc, char *argv[])
{
    context.width = 800;
    context.height = 800;
    context.title = "OpenGL Example";
    context.major = 3;
    context.minor = 3;

    if (aiv_opengl_context_init(&context))
    {
        fprintf(stderr, "error while initializing aiv_opengl_context\n");
        return -1;
    }

    
    glViewport(0, 0, context.width, context.height);
    glEnable(GL_DEPTH_TEST);

    glClearColor(0, 0, 0, 1);

    
    shader_context_t shader;
    shader.program = aiv_compile_shader("shaders/vertex.glsl", "shaders/fragment.glsl");
    if (!shader.program)
    {
        fprintf(stderr,"unable to build shader program\n");
        aiv_opengl_context_destroy(&context);
        return -1;
    }

    shader.position = glGetUniformLocation(shader.program, "position");
    shader.rotation = glGetUniformLocation(shader.program, "rotation");
    shader.scale = glGetUniformLocation(shader.program, "scale");

    shader.camera_position = glGetUniformLocation(shader.program, "camera_position");
    shader.camera_rotation = glGetUniformLocation(shader.program, "camera_rotation");

    shader.use_ortho = glGetUniformLocation(shader.program, "use_ortho");

    // shader.light_rotation = glGetUniformLocation(shader.program, "light_rotation");

    //Light
    light_t* light = light_new();
    light->light_pos_loc = glGetUniformLocation(shader.program, "light_pos");
    light->light_rot_loc = glGetUniformLocation(shader.program, "light_rot");

    mesh_t stormtrooper;
    mesh_t quadMesh;

    if (mesh_new(&stormtrooper, "Stormtrooper.obj"))
    {
        fprintf(stderr,"unable to create mesh\n");
        aiv_opengl_context_destroy(&context);
        return -1;
    }

    if (mesh_new(&quadMesh, "plane6.obj"))
    {
        fprintf(stderr,"unable to create mesh\n");
        aiv_opengl_context_destroy(&context);
        return -1;
    }

    GLuint fb1;
    glGenFramebuffers(1, &fb1);
    glBindFramebuffer(GL_FRAMEBUFFER, fb1);

    GLuint fb_color_tex;
    GLuint fb_depth_tex;

    glGenTextures(1, &fb_color_tex);
    glGenTextures(1, &fb_depth_tex);

    glBindTexture(GL_TEXTURE_2D, fb_color_tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 800, 800, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fb_color_tex, 0);

    glBindTexture(GL_TEXTURE_2D, fb_depth_tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, 800, 800, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, fb_depth_tex, 0);

    GLuint vao_debug;
    glGenVertexArrays(1, &vao_debug);

    GLfloat quad[] = {
        1, -1,
        -1, 1,
        -1, -1,
        1, 1,
        -1, 1,
        1, -1
    };

    GLfloat quad_uvs[] = {
        1, 0,
        0, 1,
        0,0,
        1,1,
        0,1,
        1,0
    };


    glBindVertexArray(vao_debug);
    GLuint quad_vbo[2];
    glGenBuffers(2, quad_vbo);

    glBindBuffer(GL_ARRAY_BUFFER, quad_vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 12, quad, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void *)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, quad_vbo[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 12, quad_uvs, GL_STATIC_DRAW);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void *)0);
    glEnableVertexAttribArray(1);

    GLuint program_debug;
    program_debug = aiv_compile_shader("shaders/vertex_quad.glsl", "shaders/fragment_quad.glsl");
    if (!program_debug)
    {
        fprintf(stderr,"unable to build shader program\n");
        aiv_opengl_context_destroy(&context);
        return -1;
    }

    float accumulator = 0;

    while(!context.closed)
    {

        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
       
        accumulator += 0.001;

        //update light each tick
        light_tick(light);

        // stormtrooper.position.x = -1;
        // stormtrooper.position.y = 0;
        // stormtrooper.position.z = 0;
        // stormtrooper.rotation.y = sin(accumulator) * 360;
        // stormtrooper.camera_position.x = 0;
        // stormtrooper.camera_position.y = 1.5;
        // stormtrooper.camera_position.z = 5;
        // stormtrooper.use_ortho = 0;

        //mesh_draw(&stormtrooper, shader);

        stormtrooper.position.x = 1;
        stormtrooper.position.y = 0;
        stormtrooper.position.z = -4;
        stormtrooper.rotation.y = sin(accumulator) * 180;
        stormtrooper.camera_position.x = 0;
        stormtrooper.camera_position.y = 1.5;
        stormtrooper.camera_position.z = 5;
        stormtrooper.use_ortho = 0;

        mesh_draw(&stormtrooper, shader);
       
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        

        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);


        glUseProgram(program_debug);
        glBindVertexArray(vao_debug);
        glBindTexture(GL_TEXTURE_2D, fb_depth_tex);

        glDrawArrays(GL_TRIANGLES, 0, 6);

        //pos, rot, scale
        quadMesh.position.x = 0;
        quadMesh.position.y = -4;
        quadMesh.position.z = -200;
        quadMesh.rotation.x = 25;
        quadMesh.scale.x = 4;
        quadMesh.scale.y = 12;
        quadMesh.scale.z = 1;
        quadMesh.camera_position.x = 0;
        quadMesh.camera_position.y = 1.5;
        quadMesh.camera_position.z = 5;
        quadMesh.use_ortho = 0;
        //light
        light->light_pos.x += 0.1;

        mesh_draw(&quadMesh, shader);
        
        mesh_draw(&stormtrooper, shader);

        stormtrooper.position.x = 1;
        stormtrooper.position.y = 0;
        stormtrooper.position.z = -4;
        stormtrooper.rotation.y = sin(accumulator) * 180;
        stormtrooper.camera_position.x = 0;
        stormtrooper.camera_position.y = 0;
        stormtrooper.camera_position.z = 0;
        stormtrooper.use_ortho = 0;

        aiv_opengl_context_swap(&context);

        glBindFramebuffer(GL_FRAMEBUFFER, fb1);

    }

    aiv_opengl_context_destroy(&context);
    light_destroy(light);

    return 0;
}
