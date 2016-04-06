#include <stdbool.h>

#define GLEW_STATIC
#include <GL/glew.h>
#include <SDL2/SDL.h>

#define WINDOW_WIDTH 960
#define WINDOW_HEIGHT 540

int main(void) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("Failed to initialize SDL: %s\n", SDL_GetError());
        return -1;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
                        SDL_GL_CONTEXT_PROFILE_CORE);

    SDL_Window *window = SDL_CreateWindow("My Game Window",
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

        SDL_GL_SwapWindow(window);
    }

    return 0;

}
