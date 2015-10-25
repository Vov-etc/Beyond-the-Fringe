#include <iostream>
#include <GL/glew.h>
#include <file.h>
#include <GL/gl.h>
#include <GL/freeglut.h>
#include <cstdio>
#include <cstring>

using namespace std;

typedef float* vec3f;
#define make_vec3f(x, y, z) new float {x, y, z}


GLuint vbo;

static void RenderSceneCB()
{
    glClear(GL_COLOR_BUFFER_BIT);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glDisableVertexAttribArray(0);

    glutSwapBuffers();
}

static void InitializeGlutCallbacks()
{
    glutDisplayFunc(RenderSceneCB);
}

static void create_buffers()
{
 /*   
    vec3f v[3];
    v[0] = make_vec3f(0, 0, 0);
    v[1] = make_vec3f(1, 0, 0);
    v[2] = make_vec3f(0, 1, 0);
   */ 
    float v[] = {0, 1, 0, 0.5, 0, 0, 0, 0.5, 0};
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(v), v, GL_STATIC_DRAW);
}

static void compile_shader(auto shader, char* type)
{
    glCompileShader(shader);
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        GLchar InfoLog[1024];
        glGetShaderInfoLog(shader, sizeof(InfoLog), NULL, InfoLog);
        fprintf(stderr, "Error compiling shader type %s: '%s'\n", type, InfoLog);
    }
}

static void check_program(auto ShaderProgram)
{
    GLint success;
    glGetProgramiv(ShaderProgram, GL_LINK_STATUS, &success);
    if (!success)
    {
        GLchar ErrorLog[1024];
        glGetProgramInfoLog(ShaderProgram, sizeof(ErrorLog), NULL, ErrorLog);
        fprintf(stderr, "Error linking shader program: '%s'\n", ErrorLog);
    }
    glValidateProgram(ShaderProgram);
}

static void create_shaders()
{
    GLuint shader_program = glCreateProgram();
    GLuint frag_sh = glCreateShader(GL_FRAGMENT_SHADER);
    GLuint vert_sh = glCreateShader(GL_VERTEX_SHADER);
    cout << GL_FRAGMENT_SHADER << ' ' << GL_VERTEX_SHADER << endl;
    const char* shaders[2];
    GLint len[2];
    shaders[0] = read_file("Shaders/fragment.glsl");
    shaders[1] = read_file("Shaders/vertex.glsl");
    len[0] = strlen(shaders[0]);
    len[1] = strlen(shaders[1]);
    glShaderSource(frag_sh, 1, shaders, len);
    glShaderSource(vert_sh, 1, shaders + 1, len + 1);
    compile_shader(frag_sh, "fragment");
    glAttachShader(shader_program, frag_sh);
    compile_shader(vert_sh, "vertex");
    glAttachShader(shader_program, vert_sh);
    glLinkProgram(shader_program);
    check_program(shader_program);
    glUseProgram(shader_program);
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(800, 600);
    glutInitWindowPosition(0, 0);

    glutCreateWindow("t1");
    InitializeGlutCallbacks();
    
    GLenum res = glewInit();
    if (res != GLEW_OK)
    {
        cerr << "Error: " << glewGetErrorString(res) << endl;
        return 1;
    }
    create_buffers();
    create_shaders();

    //glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glutMainLoop();
}
