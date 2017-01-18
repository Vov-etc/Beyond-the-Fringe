#ifdef _WIN32
    #pragma comment ( lib, "glew32" )
    #pragma comment ( lib, "opengl32" )
    #pragma comment ( lib, "freeglut" )
#endif
#include <iostream>
#include <GL\glew.h>
#include <GL\freeglut.h>


using namespace std;


int WINDOW_HEIGHT, WINDOW_WIDTH;


static void RenderSceneCB()
{
	WINDOW_HEIGHT = glutGet(GLUT_WINDOW_HEIGHT);
	WINDOW_WIDTH = glutGet(GLUT_WINDOW_WIDTH);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0, 0.0, 1.0, 0);
	glBegin(GL_TRIANGLES);
	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, 1.0f, 0.0f);  // Вверх
	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex3f(-1.0f, -1.0f, 0.0f);  // Слева снизу
	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex3f(1.0f, -1.0f, 0.0f);  // Справа снизу
	glEnd();
	glutSwapBuffers();
}


static void InitializeGlutCallbacks() {
	glutDisplayFunc(RenderSceneCB);
	glutIdleFunc(RenderSceneCB);
}


int main(int argc = 0, char** argv = NULL)
{
	FreeConsole();
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(20, 20);
	glutCreateWindow("vvv");
	//glutFullScreen();

	WINDOW_HEIGHT = glutGet(GLUT_WINDOW_HEIGHT);
	WINDOW_WIDTH = glutGet(GLUT_WINDOW_WIDTH);
	InitializeGlutCallbacks();
	GLenum res = glewInit();
	if (res != GLEW_OK)
	{
		cerr << "Error: %s\n" << glewGetErrorString(res);
		return 1;
	}
	glutMainLoop();
	return 0;
}
