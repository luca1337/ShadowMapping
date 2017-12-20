#include <aiv_opengl.h>
#include <stdio.h>
#include <stdlib.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>
#include <obj.h>


GLuint aiv_compile_shader(char *, char *);

char *_read_whole_file(char *);

aiv_opengl_context_t context;

int parse_obj_file(char *filename, obj_ctx_t *ctx)
{
        FILE *f = NULL;
#ifdef _WIN32
        if (fopen_s(&f, filename, "rb"))
                return -1;
#else
        f = fopen(filename, "rb");
#endif
        if (!f)
                return -1;
        char buf[1024];
        
        obj_parser_init(ctx);
        int ret = 0;
        char *line = NULL;
        while((line = fgets(buf, 1024, f)))
        {
                if (obj_parse_line(line, strlen(line), ctx))
                {
                        ret = -1;
                        break;
                }
        }
        fclose(f);
        return ret;

}

int main(int argc, char *argv[])
{
    context.width = 800;
    context.height = 800;
    context.title = "OpenGL Example";
    context.major = 3;
    context.minor = 3;

    obj_ctx_t ctx;

    if (aiv_opengl_context_init(&context))
    {
        fprintf(stderr, "error while initializing aiv_opengl_context\n");
        return -1;
    }

    
    glViewport(0, 0, context.width, context.height);


    GLuint program = aiv_compile_shader("shaders/vertex.glsl", "shaders/fragment.glsl");
    if (!program)
    {
        fprintf(stderr,"unable to build shader program\n");
        aiv_opengl_context_destroy(&context);
        return -1;
    }

    GLuint vao;
    glGenVertexArrays(1, &vao);
    if (vao == 0)
    {
        fprintf(stderr,"unable to create VAO\n");
        aiv_opengl_context_destroy(&context);
        return -1;
    }

    glEnable(GL_DEPTH_TEST);

    fprintf(stdout, "VAO created with id %d\n", vao);

    glClearColor(0, 0, 0, 1);

    glUseProgram(program);
    glBindVertexArray(vao);

    if (parse_obj_file("Stormtrooper.obj", &ctx))
    {
        fprintf(stderr, "unable to parse obj file\n");
        aiv_opengl_context_destroy(&context);
        return -1;
    }

    GLfloat *vertices = malloc(ctx.faces_cnt * 3 * sizeof(GLfloat) * 3);
    int pos = 0;
    for(int i=0;i<ctx.faces_cnt;i++)
    {
        vertices[pos++] = ctx.v[ctx.faces[i].v1.v_idx-1].x;
        vertices[pos++] = ctx.v[ctx.faces[i].v1.v_idx-1].y;
        vertices[pos++] = ctx.v[ctx.faces[i].v1.v_idx-1].z;

        vertices[pos++] = ctx.v[ctx.faces[i].v2.v_idx-1].x;
        vertices[pos++] = ctx.v[ctx.faces[i].v2.v_idx-1].y;
        vertices[pos++] = ctx.v[ctx.faces[i].v2.v_idx-1].z;

        vertices[pos++] = ctx.v[ctx.faces[i].v3.v_idx-1].x;
        vertices[pos++] = ctx.v[ctx.faces[i].v3.v_idx-1].y;
        vertices[pos++] = ctx.v[ctx.faces[i].v3.v_idx-1].z;
    }

    GLfloat *uvs = malloc(ctx.faces_cnt * 3 * sizeof(GLfloat) * 2);
    pos = 0;
    for(int i=0;i<ctx.faces_cnt;i++)
    {
        uvs[pos++] = ctx.vt[ctx.faces[i].v1.vt_idx-1].u;
        uvs[pos++] = 1-ctx.vt[ctx.faces[i].v1.vt_idx-1].v;
       
        uvs[pos++] = ctx.vt[ctx.faces[i].v2.vt_idx-1].u;
        uvs[pos++] = 1-ctx.vt[ctx.faces[i].v2.vt_idx-1].v;
 
        uvs[pos++] = ctx.vt[ctx.faces[i].v3.vt_idx-1].u;
        uvs[pos++] = 1-ctx.vt[ctx.faces[i].v3.vt_idx-1].v;
       
    }

    GLfloat *normals = malloc(ctx.faces_cnt * 3 * sizeof(GLfloat) * 3);
    pos = 0;
    for(int i=0;i<ctx.faces_cnt;i++)
    {
        normals[pos++] = ctx.vn[ctx.faces[i].v1.vn_idx-1].x;
        normals[pos++] = ctx.vn[ctx.faces[i].v1.vn_idx-1].y;
        normals[pos++] = ctx.vn[ctx.faces[i].v1.vn_idx-1].z;

        normals[pos++] = ctx.vn[ctx.faces[i].v2.vn_idx-1].x;
        normals[pos++] = ctx.vn[ctx.faces[i].v2.vn_idx-1].y;
        normals[pos++] = ctx.vn[ctx.faces[i].v2.vn_idx-1].z;

        normals[pos++] = ctx.vn[ctx.faces[i].v3.vn_idx-1].x;
        normals[pos++] = ctx.vn[ctx.faces[i].v3.vn_idx-1].y;
        normals[pos++] = ctx.vn[ctx.faces[i].v3.vn_idx-1].z;
    }

    GLuint vbo[3];
    glGenBuffers(3, vbo);

    // buffer 0 (v)
    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, ctx.faces_cnt * 3 * sizeof(GLfloat) * 3, vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 3, (void *)0);
    glEnableVertexAttribArray(0);

    // buffer 1 (vt)
    glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
    glBufferData(GL_ARRAY_BUFFER, ctx.faces_cnt * 3 * sizeof(GLfloat) * 2 , uvs, GL_STATIC_DRAW);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 2, (void *)0);
    glEnableVertexAttribArray(1);

    // buffer 2 (vn)
    glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
    glBufferData(GL_ARRAY_BUFFER, ctx.faces_cnt * 3 * sizeof(GLfloat) * 3, normals, GL_STATIC_DRAW);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 3, (void *)0);
    glEnableVertexAttribArray(2);

    GLuint textureId[2];
    glGenTextures(2, textureId);

    /*GLubyte bitmap[64 * 64 * 3];
    for(int i=0;i<sizeof(bitmap);i++)
        bitmap[i] = rand() % 255;

    GLubyte mask[64 * 64 * 3];
    for(int i=0;i<sizeof(bitmap)/2;i++)
        mask[i] = 255;
    for(int i=sizeof(bitmap)/2;i<sizeof(bitmap);i++)
        mask[i] = 0;*/

    int x, y, n;
    unsigned char *bitmap = stbi_load("Stormtrooper.png", &x, &y, &n, 0);//_read_whole_file("lava.tex");

    fprintf(stdout, "width %d height %d n %d\n", x, y, n);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureId[0]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, bitmap);
    // NOTE: you need to always specify the filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    /*glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, textureId[1]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, 64, 64, 0, GL_RGB, GL_UNSIGNED_BYTE, mask);
    // NOTE: you need to always specify the filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);*/

    glUniform1i(glGetUniformLocation(program, "tex"), 0);
    //glUniform1i(glGetUniformLocation(program, "tex2"), 1);

    float deltax = 2;
    float rot = 0;
    float fat = 0;

    
    unsigned char *data = malloc(800 * 800 * 3);

    GLuint fb1;
    glGenFramebuffers(1, &fb1);
    glBindFramebuffer(GL_FRAMEBUFFER, fb1);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        GLuint tex[2];
        glGenTextures(2, tex);

        // attach tex[0] to color attachment 0
        glBindTexture(GL_TEXTURE_2D, tex[0]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 800, 800, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex[0], 0);

        // attach tex[1] to depth
        glBindTexture(GL_TEXTURE_2D, tex[1]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 800, 800, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, tex[1], 0);
    }

    //glDrawBuffer(GL_COLOR_ATTACHMENT0);
    //glReadBuffer(GL_COLOR_ATTACHMENT0)

    char filename[1024];
    int frame = 0;

    while(!context.closed)
    {
        
        //glClear(GL_COLOR_BUFFER_BIT);
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

        glBindTexture(GL_TEXTURE_2D, textureId[0]);

        glUniform1f(glGetUniformLocation(program, "rotator"), rot);
        glUniform1f(glGetUniformLocation(program, "x"), -2);
        glUniform1f(glGetUniformLocation(program, "fatness"), -0.05);
      
        glDrawArrays(GL_TRIANGLES, 0, ctx.faces_cnt * 3);


        glUniform1f(glGetUniformLocation(program, "fatness"), sin(fat));
        glUniform1f(glGetUniformLocation(program, "rotator"), 0);
        glUniform1f(glGetUniformLocation(program, "x"), deltax);


        //deltax += 0.01;
        rot += 1;
        fat += 0.01;
      
        glDrawArrays(GL_TRIANGLES, 0, ctx.faces_cnt * 3);

        glBindFramebuffer(GL_READ_FRAMEBUFFER, fb1);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);


        glReadPixels(0, 0, 800, 800, GL_RGB, GL_UNSIGNED_BYTE, data);

        snprintf(filename, 1024, "screenshot_%d.bmp", frame);

        stbi_write_bmp(filename, 800, 800, 3, data);

        glBlitFramebuffer(0, 0, 800, 800, 0, 0, 800, 800, GL_COLOR_BUFFER_BIT, GL_NEAREST);

        aiv_opengl_context_swap(&context);

        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fb1);

        // enable to record a different image for each frame
        frame++;
    }

    aiv_opengl_context_destroy(&context);

    return 0;
}
