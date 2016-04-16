#include <stdbool.h>

#define GLEW_STATIC
#include <GL/glew.h>
#include <SDL2/SDL.h>

#include "common/shader.h"

#define WINDOW_WIDTH 960
#define WINDOW_HEIGHT 540

char *VERTEX_SHADER = "                                                       \
#version 330 core                                                           \n\
                                                                            \n\
layout (location = 0)                                                       \n\
in vec3 pos;                                                                \n\
                                                                            \n\
layout (location = 1)                                                       \n\
in vec3 color;                                                              \n\
                                                                            \n\
out vec3 vertex_color;                                                      \n\
                                                                            \n\
void main() {                                                               \n\
    gl_Position = vec4(pos.xyz, 1.0);                                       \n\
    vertex_color = color;                                                   \n\
}                                                                             \
";

char *FRAGMENT_SHADER = "                                                     \
#version 330 core                                                           \n\
                                                                            \n\
in vec3 vertex_color;                                                       \n\
                                                                            \n\
out vec4 color;                                                             \n\
                                                                            \n\
void main() {                                                               \n\
    color = vec4(vertex_color, 1.0f);                                       \n\
}                                                                             \
";

GLfloat VERTICES[] = {
    // Positions        // Colors
    0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, // Bottom Right
   -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, // Bottom Left
    0.0f,  0.5f, 0.0f, 0.0f, 0.0f, 1.0f, // Top
};

GLuint VAO, VBO;
GLuint PROGRAM;

static void
init(void) {
    PROGRAM = compile_program_raw(VERTEX_SHADER, FRAGMENT_SHADER);

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(VERTICES), VERTICES, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat),
                          (GLvoid *)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

#if 0
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
#endif
}

int
main(void) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("Failed to initialize SDL: %s\n", SDL_GetError());
        return -1;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
                        SDL_GL_CONTEXT_PROFILE_CORE);

    SDL_Window *window = SDL_CreateWindow("Shaders",
                                          SDL_WINDOWPOS_CENTERED,
                                          SDL_WINDOWPOS_CENTERED,
                                          WINDOW_WIDTH,
                                          WINDOW_HEIGHT,
                                          SDL_WINDOW_OPENGL);
    if (!window) {
        printf("Failed to create window: %s\n", SDL_GetError());
        return -1;
    }

    SDL_GLContext glcontext = SDL_GL_CreateContext(window);
    if (!glcontext) {
        printf("Failed to create OpenGL context: %s\n", SDL_GetError());
        return -1;
    }

    // MUST make a context AND make it current BEFORE glewInit()!
    glewExperimental = GL_TRUE;
    GLenum status = glewInit();
    if (status != GLEW_OK) {
        printf("Failed to initialize GLEW: %s\n", glewGetErrorString(status));
        return -1;
    }

    printf("OpenGL Version: %s\n", glGetString(GL_VERSION));

    GLint max_vertex_attribs;
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &max_vertex_attribs);
    printf("Maximum of vertex attributes supported: %d\n", max_vertex_attribs);

    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

    init();

    bool running = true;
    while (running) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            switch (e.type) {
                case SDL_QUIT: {
                    running = false;
                } break;

                case SDL_KEYDOWN: {
                    if (e.key.keysym.sym == SDLK_ESCAPE) {
                        running = false;
                    }
                } break;

                default: break;
            }
        }

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        GLfloat time = SDL_GetTicks() / 1000.0f;
        GLfloat green = (sin(time) / 2 ) + 0.5;

        GLint color_location = glGetUniformLocation(PROGRAM, "color");

        glUseProgram(PROGRAM);
        glUniform4f(color_location, 0.0f, green, 0.0f, 1.0f);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glBindVertexArray(0);

        SDL_GL_SwapWindow(window);
    }

    SDL_GL_DeleteContext(glcontext);
    SDL_DestroyWindow(window);

    return 0;
}
