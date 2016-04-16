#ifndef COMMON_H
#define COMMON_H

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

#endif
