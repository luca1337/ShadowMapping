#include <mesh.h>
#include <obj.h>
#include <stdio.h>

static int parse_obj_file(char *filename, obj_ctx_t *ctx)
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


int mesh_new(mesh_t *mesh, char *filename)
{
    int ret = -1;
    obj_ctx_t ctx;

    memset(mesh, 0, sizeof(mesh_t));

    mesh->scale.x = 1;
    mesh->scale.y = 1;
    mesh->scale.z = 1;

    glGenVertexArrays(1, &mesh->vao);
    if (mesh->vao == 0)
    {
        return ret;
    }

    glBindVertexArray(mesh->vao);
    
    if (parse_obj_file(filename, &ctx))
    {
        return ret;
    }

    GLfloat *vertices = malloc(ctx.faces_cnt * 3 * sizeof(GLfloat) * 3);
    if (!vertices)
        goto end0;

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
    if (!uvs)
        goto end1;

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
    if (!normals)
        goto end2;

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

    mesh->faces_cnt = ctx.faces_cnt;

    fprintf(stderr, "faces %d\n", mesh->faces_cnt);

    ret = 0;

    free(normals);
end2:
    free(uvs);
end1:
    free(vertices);
end0:
    return ret;
    
}

void mesh_draw(mesh_t *mesh, shader_context_t shader_context)
{
    glUseProgram(shader_context.program);
    glBindVertexArray(mesh->vao);

    glUniform3f(shader_context.position, mesh->position.x, mesh->position.y, mesh->position.z);
    glUniform3f(shader_context.rotation, mesh->rotation.x, mesh->rotation.y, mesh->rotation.z);
    glUniform3f(shader_context.scale, mesh->scale.x, mesh->scale.y, mesh->scale.z);

    glUniform3f(shader_context.camera_position, mesh->camera_position.x, mesh->camera_position.y, mesh->camera_position.z);
    glUniform3f(shader_context.camera_rotation, mesh->camera_rotation.x, mesh->camera_rotation.y, mesh->camera_rotation.z);

    // glUniform3f(shader_context.light_rotation, mesh->light_rotation.x, mesh->light_rotation.y, mesh->light_rotation.z);
      
    glUniform1f(shader_context.use_ortho, mesh->use_ortho);

    glDrawArrays(GL_TRIANGLES, 0, mesh->faces_cnt * 3);
}