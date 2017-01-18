#include "include/shader.h"
#include "iostream"

using namespace std;

GLuint create_shader(const char *filename, GLenum type) {
    GLchar *source = read_file(filename);
    if (source == NULL) {
        cerr << "Error while reading shader: " << filename << endl;
        return -1;
    }
    GLuint shader = glCreateShader(type);
    const GLchar *sources[2] = {
#ifdef GL_ES_VERSION_2_0
        "#version 100\n"
        "#define GLES2\n",
#else
        "#version 120\n",
#endif
        source
    };
    glShaderSource(shader, 2, sources, NULL);
    free(source);
    glCompileShader(shader);
    GLint compile_status = GL_FALSE;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compile_status);
    if (compile_status == GL_FALSE) {
        cerr << "Error while compiling shader: " << filename << endl;
        glDeleteShader(shader);
        return -1;
    }
    return shader;
}

GLuint create_program(GLuint vs, GLuint fs) {
    GLuint prog = glCreateProgram();
    glAttachShader(prog, vs);
    glAttachShader(prog, fs);
    glLinkProgram(prog);
    GLint link_status = GL_FALSE;
    glGetProgramiv(prog, GL_LINK_STATUS, &link_status);
    if (link_status == GL_FALSE) {
        cerr << "Error while linking program" << endl;
        glDeleteProgram(prog);
        return -1;
    }
    
    GLint Success = GL_FALSE;
    glValidateProgram(prog);
    glGetProgramiv(prog, GL_VALIDATE_STATUS, &Success);
    GLchar ErrLog[1024];
    if (!Success) {
        glGetProgramInfoLog(prog, sizeof(ErrLog), NULL, ErrLog);
        cerr << "Invalid shader program: " << ErrLog << endl;
        exit(1);
    }
    return prog;
}
