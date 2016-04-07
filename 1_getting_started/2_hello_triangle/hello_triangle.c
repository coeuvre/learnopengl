#include <stdbool.h>

#define GLEW_STATIC
#include <GL/glew.h>
#include <SDL2/SDL.h>

#define WINDOW_WIDTH 960
#define WINDOW_HEIGHT 540

char *VERTEX_SHADER = "                                                     \
#version 330 core                                                         \n\
                                                                          \n\
layout (location = 0)                                                     \n\
in vec3 pos;                                                              \n\
                                                                          \n\
void main() {                                                             \n\
    gl_Position = vec4(pos.xyz, 1.0);                                     \n\
}                                                                           \
";

char *FRAGMENT_SHADER = "                                                   \
#version 330 core                                                         \n\
                                                                          \n\
out vec4 color;                                                           \n\
                                                                          \n\
void main() {                                                             \n\
    color = vec4(1.0, 0.5, 0.2, 1.0);                                     \n\
}                                                                           \
";

GLfloat VERTICES[] = {
    0.5f,  0.5f, 0.0f, // Top Right
    0.5f, -0.5f, 0.0f, // Bottom Right
   -0.5f, -0.5f, 0.0f, // Bottom Left
   -0.5f,  0.5f, 0.0f, // Top Left
};

GLuint INDICES[] = {
    0, 1, 3, // First Triangle
    1, 2, 3, // Second Triangle
};

GLuint VAO, VBO, EBO;
GLuint PROGRAM;

static GLuint
compile_shader_raw(GLenum type, const char *source) {
    GLuint result = glCreateShader(type);

    glShaderSource(result, 1, &source, 0);

    glCompileShader(result);

    GLint success;
    glGetShaderiv(result, GL_COMPILE_STATUS, &success);
    if (success != GL_TRUE) {
        result = 0;

        char buf[512];
        glGetShaderInfoLog(result, sizeof(buf), 0, buf);
        printf("Failed to compile shader: %s\n", buf);
    }

    return result;
}

static GLuint
compile_program_raw(char *vertex_shader_source, char *fragment_shader_source) {
    GLuint result = 0;

    GLuint vertex_shader = compile_shader_raw(GL_VERTEX_SHADER,
                                              vertex_shader_source);
    if (vertex_shader) {
        GLuint fragment_shader = compile_shader_raw(GL_FRAGMENT_SHADER,
                                                    fragment_shader_source);
        if (fragment_shader) {
            result = glCreateProgram();
            glAttachShader(result, vertex_shader);
            glAttachShader(result, fragment_shader);
            glLinkProgram(result);

            GLint success;
            glGetProgramiv(result, GL_LINK_STATUS, &success);
            if (success != GL_TRUE) {
                result = 0;

                char buf[512];
                glGetProgramInfoLog(result, sizeof(buf), 0, buf);
                printf("Failed to link program: %s\n", buf);
            }
        }
    }

    return result;
}

static void
init(void) {
    PROGRAM = compile_program_raw(VERTEX_SHADER, FRAGMENT_SHADER);

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(VERTICES), VERTICES, GL_STATIC_DRAW);

    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(INDICES), INDICES,
                 GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);

#if 1
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
#endif
}

int main(void) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("Failed to initialize SDL: %s\n", SDL_GetError());
        return -1;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
                        SDL_GL_CONTEXT_PROFILE_CORE);

    SDL_Window *window = SDL_CreateWindow("Hello Triangle",
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
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        SDL_GL_SwapWindow(window);
    }

    SDL_GL_DeleteContext(glcontext);
    SDL_DestroyWindow(window);

    return 0;
}
