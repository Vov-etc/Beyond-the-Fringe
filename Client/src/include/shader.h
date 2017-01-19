#ifndef SHADER_H
#define SHADER_H

#include "file.h"
#include <GL/glew.h>
#include <GL/freeglut.h>

GLuint create_shader(const char *filename, GLenum type);
GLuint create_program(GLuint vs, GLuint fs);
#endif //SHADER_H
