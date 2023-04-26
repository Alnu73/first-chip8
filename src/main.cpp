#include <stdio.h>

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <chrono>
#include <thread>
#include "chip8.h"

#define SCREEN_WIDTH 64
#define SCREEN_HEIGHT 32

chip8 myChip;
int modifier = 10;
int display_width = SCREEN_WIDTH * modifier;
int display_height = SCREEN_HEIGHT * modifier;

void display();
void keyboard_up(unsigned char key, int x, int y);
void reshape_window(GLsizei w, GLsizei h);
void keyboard_down(unsigned char key, int x, int y);

typedef uint8_t u8;
u8 screenData[SCREEN_HEIGHT][SCREEN_WIDTH][3];
void setupTexture();

int main(int argc, char** argv)
{
	if (argc < 2)
	{
		printf("Usage: ./chip8 ./games/<gamename>.c8\n\n");
		return 1;
	}

	// Load game
	if (!myChip.load_application(argv[1])) {
		return 1;
}
	//GLUT initialization
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);		//Colour mode RGBA, double buffer
	glutInitWindowSize(display_width, display_height);
	glutInitWindowPosition(320, 320);
	glutCreateWindow("CHIP-8 Emulator");

	glutDisplayFunc(display);			//Specifies the display callback for the current window
	glutIdleFunc(display);
	glutReshapeFunc(reshape_window);
	//Keyboard callbacks
	glutKeyboardFunc(keyboard_down);
	glutKeyboardUpFunc(keyboard_up);

	setupTexture();

	glutMainLoop();

	return 0;
}

void updateTexture(const chip8& c8)
{
	// Update pixels
	for (int y = 0; y < 32; ++y)
		for (int x = 0; x < 64; ++x)
			if (c8.gfx[(y * 64) + x] == 0)
				screenData[y][x][0] = screenData[y][x][1] = screenData[y][x][2] = 0;	// Disabled
			else
				screenData[y][x][0] = screenData[y][x][1] = screenData[y][x][2] = 255;  // Enabled

	// Update Texture
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, GL_RGB, GL_UNSIGNED_BYTE, (GLvoid*)screenData);

	glBegin(GL_QUADS);
	glTexCoord2d(0.0, 0.0);		glVertex2d(0.0, 0.0);
	glTexCoord2d(1.0, 0.0); 	glVertex2d(display_width, 0.0);
	glTexCoord2d(1.0, 1.0); 	glVertex2d(display_width, display_height);
	glTexCoord2d(0.0, 1.0); 	glVertex2d(0.0, display_height);
	glEnd();
}

void display() {
	myChip.emulate_cycle();

	if (myChip.draw_flag) {
		glClear(GL_COLOR_BUFFER_BIT);

		updateTexture(myChip);
		std::this_thread::sleep_for(std::chrono::milliseconds(20));

		glutSwapBuffers();

		myChip.draw_flag = false;
	}
}

void setupTexture()
{
	// Clear screen
	for (int y = 0; y < SCREEN_HEIGHT; ++y)
		for (int x = 0; x < SCREEN_WIDTH; ++x)
			screenData[y][x][0] = screenData[y][x][1] = screenData[y][x][2] = 0;

	// Create a texture 
	glTexImage2D(GL_TEXTURE_2D, 0, 3, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, (GLvoid*)screenData);

	// Set up the texture
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	// Enable textures
	glEnable(GL_TEXTURE_2D);
}

void reshape_window(GLsizei w, GLsizei h)
{
	glClearColor(0.0f, 0.0f, 0.5f, 0.0f);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, w, h, 0);
	glMatrixMode(GL_MODELVIEW);
	glViewport(0, 0, w, h);

	// Resize quad
	display_width = w;
	display_height = h;
}

void keyboard_down(unsigned char key, int x, int y) {
	if (key == 27)    // esc
		exit(0);

	if (key == '1')		myChip.keys[0x1] = 1;
	else if (key == '2')	myChip.keys[0x2] = 1;
	else if (key == '3')	myChip.keys[0x3] = 1;
	else if (key == '4')	myChip.keys[0xC] = 1;

	else if (key == 'q')	myChip.keys[0x4] = 1;
	else if (key == 'w')	myChip.keys[0x5] = 1;
	else if (key == 'e')	myChip.keys[0x6] = 1;
	else if (key == 'r')	myChip.keys[0xD] = 1;

	else if (key == 'a')	myChip.keys[0x7] = 1;
	else if (key == 's')	myChip.keys[0x8] = 1;
	else if (key == 'd')	myChip.keys[0x9] = 1;
	else if (key == 'f')	myChip.keys[0xE] = 1;

	else if (key == 'z')	myChip.keys[0xA] = 1;
	else if (key == 'x')	myChip.keys[0x0] = 1;
	else if (key == 'c')	myChip.keys[0xB] = 1;
	else if (key == 'v')	myChip.keys[0xF] = 1;
}

void keyboard_up(unsigned char key, int x, int y) {
	if (key == '1')		myChip.keys[0x1] = 0;
	else if (key == '2')	myChip.keys[0x2] = 0;
	else if (key == '3')	myChip.keys[0x3] = 0;
	else if (key == '4')	myChip.keys[0xC] = 0;

	else if (key == 'q')	myChip.keys[0x4] = 0;
	else if (key == 'w')	myChip.keys[0x5] = 0;
	else if (key == 'e')	myChip.keys[0x6] = 0;
	else if (key == 'r')	myChip.keys[0xD] = 0;

	else if (key == 'a')	myChip.keys[0x7] = 0;
	else if (key == 's')	myChip.keys[0x8] = 0;
	else if (key == 'd')	myChip.keys[0x9] = 0;
	else if (key == 'f')	myChip.keys[0xE] = 0;

	else if (key == 'z')	myChip.keys[0xA] = 0;
	else if (key == 'x')	myChip.keys[0x0] = 0;
	else if (key == 'c')	myChip.keys[0xB] = 0;
	else if (key == 'v')	myChip.keys[0xF] = 0;
}