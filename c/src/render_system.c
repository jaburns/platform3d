#include "render_system.h"

#include "gl.h"
#include "components.h"
#include "resources.h"

static const GLfloat triangle_vertices[] = {
    0.0f,  0.577f, 0.0f,
    0.5f, -0.289f, 0.0f,
   -0.5f, -0.289f, 0.0f,
};

static const GLfloat triangle_colors[] = {
    1.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 1.0f,
};

struct RenderSystem
{
    GLuint vao;
    GLuint position_buffer;
    GLuint color_buffer;
    ShaderProgramRef shader;
};

RenderSystem *rendersystem_new(void)
{
    RenderSystem *sys = malloc(sizeof(RenderSystem));

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_MULTISAMPLE);
    glFrontFace(GL_CW);
    glCullFace(GL_BACK);
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

    sys->shader = resources_load_shader("resources/shaders/colors.vert", "resources/shaders/colors.frag");

    glGenVertexArrays(1, &sys->vao);
    glBindVertexArray(sys->vao);

    glGenBuffers(1, &sys->position_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, sys->position_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(triangle_vertices), &triangle_vertices, GL_STATIC_DRAW);

    const GLint position_prop = glGetAttribLocation(sys->shader, "position");
    glEnableVertexAttribArray(position_prop);
    glVertexAttribPointer(position_prop, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, NULL);

    glGenBuffers(1, &sys->color_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, sys->color_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(triangle_colors), &triangle_colors, GL_STATIC_DRAW);

    const GLint color_prop = glGetAttribLocation(sys->shader, "color");
    glEnableVertexAttribArray(color_prop);
    glVertexAttribPointer(color_prop, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, NULL);

    return sys;
}

void rendersystem_run(RenderSystem *sys, ECS *ecs)
{
    Entity camera_entity;
    if (!ECS_FIND_FIRST_ENTITY_WITH_COMPONENT(Camera, ecs, &camera_entity)) return;

    ECS_GET_COMPONENT_DECL(Camera, camera, ecs, camera_entity);
    ECS_GET_COMPONENT_DECL(Transform, camera_transform, ecs, camera_entity);

    mat4x4 projection;
    mat4x4_perspective(projection, camera->fov, 1.f, camera->near, camera->far);

//  size_t num_teapots;
//  Entity *teapots = ECS_FIND_ALL_ENTITIES_WITH_COMPONENT_ALLOC(Teapot, ecs, &num_teapots);

//  for (int i = 0; i < num_teapots; ++i)
//  {
//      ECS_GET_COMPONENT_DECL(Transform, teapot_transform, ecs, teapots[i]);
//  }

    mat4x4 view;
    mat4x4_look_at(view, (vec3){0.f,0.f,2.f}, (vec3){0.f,0.f,0.f}, (vec3){0.f,1.f,0.f});

    mat4x4 model;
    mat4x4_identity(model);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(sys->shader);
    glUniformMatrix4fv(glGetUniformLocation(sys->shader, "model"), 1, GL_FALSE, model);
    glUniformMatrix4fv(glGetUniformLocation(sys->shader, "view"), 1, GL_FALSE, view);
    glUniformMatrix4fv(glGetUniformLocation(sys->shader, "projection"), 1, GL_FALSE, projection);

    glBindVertexArray(sys->vao);

    glDrawArrays(GL_TRIANGLES, 0, 3);
}

void rendersystem_delete(RenderSystem *sys)
{
    if (!sys) return;

    glBindVertexArray(0);

    glDeleteBuffers(1, &sys->position_buffer);
    glDeleteBuffers(1, &sys->color_buffer);
    glDeleteVertexArrays(1, &sys->vao);

    resources_delete_shader(sys->shader);

    free(sys);
}