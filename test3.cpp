#ifdef _WIN32
#include <windows.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>
#include <math.h>
#define GL_SILENCE_DEPRECATION

#define XMAX 1200
#define YMAX 700
#define SPACESHIP_SPEED 20
#define TOP 0
#define RIGHT 1
#define BOTTOM 2
#define LEFT 3

GLint m_viewport[4];
bool mButtonPressed = false;
float mouseX, mouseY;
enum view
{
	INTRO,
	MENU,
	INSTRUCTIONS,
	GAME,
	GAMEOVER
};
view viewPage = MENU; // initial value
bool keyStates[256] = {false};
bool direction[4] = {false};
int alienLife1 = 100;
int alienLife2 = 100;
bool gameOver = false;
float xOne = 500, yOne = 0;
float xTwo = 500, yTwo = 0;
bool laser1 = false, laser2 = false;
GLint CI = 0;
GLfloat a[][2] = {0, -50, 70, -50, 70, 70, -70, 70};
GLfloat LightColor[][3] = {1, 1, 0, 0, 1, 1, 0, 1, 0};

void displayRasterText(float x, float y, float z, char *stringToDisplay)
{
	glRasterPos3f(x, y, z);
	for (char *c = stringToDisplay; *c != '\0'; c++)
	{
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, *c);
	}
}

void introScreen()
{
	glClear(GL_COLOR_BUFFER_BIT);
	displayRasterText(-300, -550, 0.0, "Press ENTER to start the game");
	glFlush();
	glutSwapBuffers();
}

void startScreenDisplay()
{
	glLineWidth(1);

	glColor3f(1, 1, 0);
	glBegin(GL_POLYGON); // START GAME PLOYGON
	glVertex2f(-200, 300);
	glVertex2f(-200, 400);
	glVertex2f(200, 400);
	glVertex2f(200, 300);
	glEnd();

	if (mouseX >= -100 && mouseX <= 100 && mouseY >= 150 && mouseY <= 200)
	{
		glColor3f(0, 0, 1);
		if (mButtonPressed)
		{
			alienLife1 = alienLife2 = 100;
			viewPage = GAME;
			mButtonPressed = false;
		}
	}
	else
		glColor3f(0, 0, 0);

	displayRasterText(-100, 340, 0.4, "Start Game");
	glutPostRedisplay();
}
void DrawSpaceshipBody(bool isPlayer1)
{
	if (isPlayer1)
		glColor3f(1, 0, 0); // BASE
	else
		glColor3f(0.5, 0, 0.5);

	glPushMatrix();
	glScalef(70, 20, 1);
	glutSolidSphere(1, 50, 50);
	glPopMatrix();

	glPushMatrix(); // LIGHTS
	glScalef(3, 3, 1);
	glTranslated(-20, 0, 0); // 1
	glColor3fv(LightColor[(CI + 0) % 3]);
	glutSolidSphere(1, 300, 300);
	glTranslated(5, 0, 0); // 2
	glColor3fv(LightColor[(CI + 1) % 3]);
	glutSolidSphere(1, 300, 300);
	glTranslated(5, 0, 0); // 3
	glColor3fv(LightColor[(CI + 2) % 3]);
	glutSolidSphere(1, 300, 300);
	glTranslated(5, 0, 0); // 4
	glColor3fv(LightColor[(CI + 0) % 3]);
	glutSolidSphere(1, 300, 300);
	glTranslated(5, 0, 0); // 5
	glColor3fv(LightColor[(CI + 1) % 3]);
	glutSolidSphere(1, 300, 300);
	glTranslated(5, 0, 0); // 6
	glColor3fv(LightColor[(CI + 2) % 3]);
	glutSolidSphere(1, 300, 300);
	glTranslated(5, 0, 0); // 7
	glColor3fv(LightColor[(CI + 0) % 3]);
	glutSolidSphere(1, 300, 300);
	glTranslated(5, 0, 0); // 8
	glColor3fv(LightColor[(CI + 1) % 3]);
	glutSolidSphere(1, 300, 300);
	glTranslated(5, 0, 0); // 9
	glColor3fv(LightColor[(CI + 2) % 3]);
	glutSolidSphere(1, 300, 300);

	glPopMatrix();
}

void DrawSpaceshipDoom()
{
	glColor4f(0.7, 1, 1, 0.0011);
	glPushMatrix();
	glTranslated(0, 30, 0);
	glScalef(35, 50, 1);
	glutSolidSphere(1, 50, 50);
	glPopMatrix();
}

void DrawLaser(int x, int y)
{
	int xend = -XMAX, yend = y;
	glLineWidth(5);
	glColor3f(1, 0, 0);
	glBegin(GL_LINES);
	glVertex2f(x, y);
	glVertex2f(xend, yend);
	glEnd();
}

void SpaceshipCreate(int x, int y, bool isPlayer1)
{
	glPushMatrix();
	glTranslated(x, y, 0);
	DrawSpaceshipDoom();
	glPushMatrix();
	glTranslated(4, 19, 0);
	glPopMatrix();
	DrawSpaceshipBody(isPlayer1);
	glEnd();
	glPopMatrix();
}

void DisplayHealthBar1()
{
	char temp1[40];
	glColor3f(1, 1, 1);
	sprintf(temp1, "  LIFE = %d", alienLife1);
	displayRasterText(-1100, 600, 0.4, temp1);
	glColor3f(1, 0, 0);
}

void DisplayHealthBar2()
{
	char temp2[40];
	glColor3f(1, 1, 1);
	sprintf(temp2, "  LIFE = %d", alienLife2);
	displayRasterText(800, 600, 0.4, temp2);
	glColor3f(1, 0, 0);
}

void checkLaserContact(int x, int y, int xp, int yp, bool player1)
{
	int xend = -XMAX, yend = y;
	xp += 8;
	yp += 8; // moving circle slightly up to fix laser issue

	float m = (float)(yend - y) / (float)(xend - x);
	float k = y - m * x;
	int r = 50; // approx radius of the spaceship

	float b = 2 * xp - 2 * m * (k - yp);
	float a = 1 + m * m;
	float c = xp * xp + (k - yp) * (k - yp) - r * r;

	float d = (b * b - 4 * a * c); // discriminant for the equation
	if (d >= 0)
	{
		if (player1)
			alienLife1 -= 5;
		else
			alienLife2 -= 5;
	}
}

void gameScreenDisplay()
{
	DisplayHealthBar1();
	DisplayHealthBar2();
	glScalef(2, 2, 0);

	if (alienLife1 > 0)
	{
		SpaceshipCreate(xOne, yOne, true);
		if (laser1)
		{
			DrawLaser(xOne, yOne);
			checkLaserContact(xOne, yOne, -xTwo, yTwo, true);
		}
	}
	else
	{
		viewPage = GAMEOVER;
	}

	if (alienLife2 > 0)
	{
		glPushMatrix();
		glScalef(-1, 1, 1);
		SpaceshipCreate(xTwo, yTwo, false);
		if (laser2)
		{
			DrawLaser(xTwo, yTwo);
			checkLaserContact(xTwo, yTwo, -xOne, yOne, false);
		}
		glPopMatrix();
	}
	else
	{
		viewPage = GAMEOVER;
	}

	if (viewPage == GAMEOVER)
	{
		xOne = xTwo = 500;
		yOne = yTwo = 0;
	}
}

void displayGameOverMessage()
{
	glColor3f(1, 1, 0);
	char *message;
	if (alienLife1 > 0)
		message = "Game Over! Player 1 won the game";
	else
		message = "Game Over! Player 2 won the game";

	displayRasterText(-350, 600, 0.4, message);
}

void keyOperations()
{
	if (keyStates[13] == true && viewPage == INTRO)
	{
		viewPage = MENU;
		printf("view value changed to %d", viewPage);
		printf("enter key pressed\n");
	}
	if (viewPage == GAME)
	{
		laser1 = false;
		laser2 = false;

		if (keyStates['m'] == true)
			laser1 = true;
		if (keyStates['c'] == true)
			laser2 = true;

		if (!laser1)
		{
			if (keyStates['l'] == true)
				xOne += SPACESHIP_SPEED;
			if (keyStates['j'] == true)
				xOne -= SPACESHIP_SPEED;
			if (keyStates['i'] == true)
				yOne += SPACESHIP_SPEED;
			if (keyStates['k'] == true)
				yOne -= SPACESHIP_SPEED;
		}

		if (!laser2)
		{
			if (keyStates['d'] == true)
				xTwo -= SPACESHIP_SPEED;
			if (keyStates['a'] == true)
				xTwo += SPACESHIP_SPEED;
			if (keyStates['w'] == true)
				yTwo += SPACESHIP_SPEED;
			if (keyStates['s'] == true)
				yTwo -= SPACESHIP_SPEED;
		}
	}
}

void passiveMotionFunc(int x, int y)
{
	mouseX = float(x) / (m_viewport[2] / 1200.0) - 600.0;
	mouseY = -(float(y) / (m_viewport[3] / 700.0) - 350.0);
	glutPostRedisplay();
}

void mouseClick(int buttonPressed, int state, int x, int y)
{
	if (buttonPressed == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
		mButtonPressed = true;
	else
		mButtonPressed = false;
	glutPostRedisplay();
}

void keyPressed(unsigned char key, int x, int y)
{
	keyStates[key] = true;
	glutPostRedisplay();
}

void refresh()
{
	glutPostRedisplay();
}

void keyReleased(unsigned char key, int x, int y)
{
	keyStates[key] = false;
}


//STAR FUNCTIONALITY TEST

#define NUM_STARS 100
#define TWINKLE_SPEED 0.01

float starPositions[NUM_STARS][2];
float starColors[NUM_STARS][3];
float twinkle[NUM_STARS];

void initStars()
{
    // Initialize star positions and colors
    for (int i = 0; i < NUM_STARS; i++)
    {
        starPositions[i][0] = (float)(rand() % 2400 - 1200);
        starPositions[i][1] = (float)(rand() % 1400 - 700);
        starColors[i][0] = (float)(rand() % 256) / 255.0;
        starColors[i][1] = (float)(rand() % 256) / 255.0;
        starColors[i][2] = (float)(rand() % 256) / 255.0;
        twinkle[i] = (float)rand() / RAND_MAX; // Random brightness between 0.0 and 1.0
    }
}

void drawDiamond(float x, float y, float size)
{
    glBegin(GL_POLYGON);
    glVertex2f(x - size, y);
    glVertex2f(x, y + size);
    glVertex2f(x + size, y);
    glVertex2f(x, y - size);
    glEnd();
}

void drawStars()
{
    float diamondSize = 5.0; // Adjust the size of the diamonds

    for (int i = 0; i < NUM_STARS; i++)
    {
        glColor3f(starColors[i][0] * twinkle[i], starColors[i][1] * twinkle[i], starColors[i][2] * twinkle[i]);
        drawDiamond(starPositions[i][0], starPositions[i][1], diamondSize);

        // Update twinkle value
        twinkle[i] += TWINKLE_SPEED;
        if (twinkle[i] > 1.0)
        {
            twinkle[i] = 0.0; // Reset twinkle value if it exceeds 1.0
        }
    }
}

// Display, init, main

void display()
{
	keyOperations();
	glClear(GL_COLOR_BUFFER_BIT);

    // Draw Stars
    drawStars();

	switch (viewPage)
	{
	case MENU:
		startScreenDisplay();
		break;
	case GAME:
		gameScreenDisplay();
		// reset scaling values
		glScalef(1 / 2, 1 / 2, 0);
		break;
	case GAMEOVER:
		displayGameOverMessage();
		startScreenDisplay();
		break;
	}

	glFlush();
	glLoadIdentity();
	glutSwapBuffers();
}

void init()
{
	glClearColor(0.0, 0.0, 0.0, 0);
	glColor3f(1.0, 0.0, 0.0);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluOrtho2D(-1200, 1200, -700, 700); //<-----CHANGE THIS TO GET EXTRA SPACE
	glMatrixMode(GL_MODELVIEW);

    // intialize stars 
    initStars();
}

int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(1200, 600);
	glutCreateWindow("Space Shooter");
	init();
	glutIdleFunc(refresh);
	glutKeyboardFunc(keyPressed);
	glutKeyboardUpFunc(keyReleased);
	glutMouseFunc(mouseClick);
	glutPassiveMotionFunc(passiveMotionFunc);
	glGetIntegerv(GL_VIEWPORT, m_viewport);

	viewPage = MENU; // Set the initial view to MENU

	glutDisplayFunc(display);
	glutMainLoop();
}
