#include <aiv_opengl.h>
#include <stdio.h>
#include <stdlib.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

aiv_opengl_context_t context;

GLuint aiv_compile_shader(char *, char *);

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

    size_t nframes = 128;
    int width = 800;
    int height = 800;
    unsigned char **frames = malloc(nframes * sizeof(unsigned char *));

    for(int i=0;i<nframes;i++)
    {
        char filename[1024];
        snprintf(filename, 1024, "screenshot_%d.bmp", i);
        int x, y, n;
        frames[i] = stbi_load(filename, &x, &y, &n, 0);
    }

    GLuint tex;
    glGenTextures(1, &tex);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    // NOTE: you need to always specify the filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    GLuint fb1;
    glGenFramebuffers(1, &fb1);
    glBindFramebuffer(GL_FRAMEBUFFER, fb1);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex, 0);

    GLuint program = aiv_compile_shader("shaders/vertex_sprite.glsl", "shaders/fragment_sprite.glsl");
    if (!program)
    {
        fprintf(stderr,"unable to build shader program\n");
        aiv_opengl_context_destroy(&context);
        return -1;
    }

    glUseProgram(program);

    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    GLuint vbo;
    glGenBuffers(1, &vbo);

    GLfloat quad[] = {
        1, 1, 1, 1,
        -1, 1, 0, 1,
        -1, -1, 0, 0,
        1, -1, 1, 0,
        1, 1, 1, 1,
        -1, -1, 0, 0
    };

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * sizeof(quad), quad, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 4, (void *)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 4, (void *) (sizeof(GLfloat) * 2));
    glEnableVertexAttribArray(1);

    int current_frame = 0;
    
    while(!context.closed)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, fb1);

        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT);

        //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, frames[current_frame]);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, frames[current_frame]);
        
        //glBindFramebuffer(GL_READ_FRAMEBUFFER, fb1);
        //glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

        //glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glClearColor(1, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT);

        glDrawArrays(GL_TRIANGLES, 0, 6);

        aiv_opengl_context_swap(&context);

        current_frame++;
        if (current_frame >= nframes)
            current_frame = 0;
    }

    aiv_opengl_context_destroy(&context);

    return 0;
}
