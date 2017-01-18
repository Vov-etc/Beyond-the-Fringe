#include <GL/glew.h>
#include <GL/freeglut.h>
#include "file.h"
#ifndef SHADER
#define SHADER

GLuint create_shader(const char *filename, GLenum type);
GLuint create_program(GLuint vs, GLuint fs);
#endif //SHADER
