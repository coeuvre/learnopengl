#include <stdbool.h>

#define GLEW_STATIC
#include <GL/glew.h>
#include <SDL2/SDL.h>

#include <SOIL/SOIL.h>

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
layout (location = 2)                                                       \n\
in vec2 texcoord;                                                           \n\
                                                                            \n\
out vec3 vertex_color;                                                      \n\
out vec2 vertex_texcoord;                                                   \n\
                                                                            \n\
void main() {                                                               \n\
    gl_Position = vec4(pos.xyz, 1.0);                                       \n\
    vertex_color = color;                                                   \n\
    vertex_texcoord = texcoord;                                             \n\
}                                                                             \
";

char *FRAGMENT_SHADER = "                                                     \
#version 330 core                                                           \n\
                                                                            \n\
uniform sampler2D texture0;                                                 \n\
uniform sampler2D texture1;                                                 \n\
                                                                            \n\
in vec3 vertex_color;                                                       \n\
in vec2 vertex_texcoord;                                                    \n\
                                                                            \n\
out vec4 color;                                                             \n\
                                                                            \n\
void main() {                                                               \n\
    color = mix(texture(texture0, vertex_texcoord),                         \n\
                texture(texture1, vertex_texcoord),                         \n\
                0.2);                                                       \n\
}                                                                             \
";

GLfloat VERTICES[] = {
    // Positions       // Colors         // Texture Coords
    0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // Top Right
    0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // Bottom Right
   -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // Bottom Left
   -0.5f,  0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, // Top Left
};

GLuint INDICES[] = {
    0, 1, 3, // First Triangle
    1, 2, 3, // Second Triangle
};

GLuint VAO, VBO, EBO;
GLuint PROGRAM;

GLuint TEXTURE0, TEXTURE1;

static void
init(void) {
    PROGRAM = compile_program_raw(VERTEX_SHADER, FRAGMENT_SHADER);

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(VERTICES), VERTICES, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat),
                          (GLvoid *)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat),
                          (GLvoid *)(6 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);

    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(INDICES), INDICES,
                 GL_STATIC_DRAW);

    glBindVertexArray(0);

#if 0
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
#endif

    int image_width, image_height;
    unsigned char *image = SOIL_load_image("container.jpg",
                                           &image_width,
                                           &image_height,
                                           0, SOIL_LOAD_RGB);
    glGenTextures(1, &TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, TEXTURE0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image_width, image_height, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, image);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
    SOIL_free_image_data(image);

    image = SOIL_load_image("awesomeface.png", &image_width, &image_height, 0,
                            SOIL_LOAD_RGB);
    glGenTextures(1, &TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, TEXTURE1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image_width, image_height, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, image);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
    SOIL_free_image_data(image);

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

    SDL_Window *window = SDL_CreateWindow("Textures",
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

        glUseProgram(PROGRAM);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, TEXTURE0);
        glUniform1i(glGetUniformLocation(PROGRAM, "texture0"), 0);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, TEXTURE1);
        glUniform1i(glGetUniformLocation(PROGRAM, "texture1"), 1);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        SDL_GL_SwapWindow(window);
    }

    SDL_GL_DeleteContext(glcontext);
    SDL_DestroyWindow(window);

    return 0;
}
