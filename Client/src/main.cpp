#pragma comment (lib, "glew32")
#pragma comment (lib, "opengl32")
#pragma comment (lib, "freeglut")

#include <fstream>
#include <iostream>
#include <vector>
#include <GL/glew.h>
#include <GL/glut.h>

#include "include/shader.h"
#include "include/net.h"
#include "include/controls.h"


using namespace std;

struct vec {
    float x, y;
    vec(float x1 = 0, float y1 = 0):
        x(x1),
        y(y1)
        {}
};

void display();
void special(int key, int x, int y);
void special_up(int key, int x, int y);
void keyboard(unsigned char key, int x, int y);
void keyboard_up(unsigned char key, int x, int y);
void update();
void passive_motion(int x, int y);

GLuint ShaderProg, VBO, move_location;
Net net;
Controls kbd;

void init_res() {
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    vector <vec> vert(4);
    vert[0] = vec(1, 1);
    vert[1] = vec(1, -1);
    vert[2] = vec(-1, -1);
    vert[3] = vec(-1, 1);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vec) * vert.size(), vert.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    GLuint vs = create_shader("res/Vertex.hlsl", GL_VERTEX_SHADER);
    GLuint fs = create_shader("res/Pixel.hlsl", GL_FRAGMENT_SHADER);
    ShaderProg = create_program(vs, fs);
    glUseProgram(ShaderProg);
    move_location = glGetUniformLocation(ShaderProg, "move");
}


void init_callback() {
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutKeyboardUpFunc(keyboard_up);
    glutSpecialFunc(special);
    glutSpecialUpFunc(special_up);
    glutPassiveMotionFunc(passive_motion);
    glutIdleFunc(update);
}


int main(int argc, char * argv[])
{ 
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA); 
    glutInitWindowSize(800, 600);
    
    glutCreateWindow("OpenGL lesson 1");
    GLenum res = glewInit();
    if (res != GLEW_OK)
    {
        cerr << "Glew: " << glewGetErrorString(res) << endl;
        return 1;
    }
   
    init_res();
    init_callback();
    net.update(MSG_HELLO, kbd);
    glutMainLoop();

    return 0;
}


void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    /*glBegin(GL_QUADS);
    glColor3f(1.0, 1.0, .0);
    glVertex2i(250, 450);
    glColor3f(0.0, 0.0, 1.0);
    glVertex2i(250, 150);
    glColor3f(0.0, 1.0, 0.0);
    glVertex2i(550, 150);
    glColor3f(1.0, 0.0, 0.0);
    glVertex2i(550, 450);
    glEnd();*/
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    vec move(0.1, 0.1);
    glUniform2fv(move_location, 1, (float*)&move);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glDrawArrays(GL_QUADS, 0, 4);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDisableVertexAttribArray(0);
    glutSwapBuffers();
    return;
}

void special(int key, int x, int y) {
    kbd.set_mouse_pos(x, y);
    kbd.push(key + 256);
}

void special_up(int key, int x, int y) {
    kbd.set_mouse_pos(x, y);
    kbd.erase(key + 256);
}

void keyboard(unsigned char key, int x, int y) {
    kbd.set_mouse_pos(x, y);
    kbd.push(key);
}

void keyboard_up(unsigned char key, int x, int y) {
    kbd.set_mouse_pos(x, y);
    kbd.erase(key);
}

void passive_motion(int x, int y) {
    kbd.set_mouse_state(false);
    kbd.set_mouse_pos(x, y);
}

void update() {
    for (auto el : kbd.to_vec()) {
        cout << el << " ";
    }
    cout << endl;
    net.update(MSG_KEYS_DOWN, kbd);
}
