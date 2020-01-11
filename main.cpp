#define _USE_MATH_DEFINES
#pragma comment(lib, "irrklang.lib")

#include <cmath>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include <windows.h>
#include <GL/gl.h>
#include "glut.h"
#include "creature.h"
#include "pac.h"
#include "gameboard.h"
#include "scrnsave.h"
#include "ghost.h"
#include "TrailParticle.h"
#include "ParticleSystem.h"
#include "Mocap.h"
#include "Time.h"
#include "SoundManager.h"
#include <iostream>
#include <string>
#include <cstdlib>
#include <time.h>
#include "BreakableWall.h"


#define GHOST_SCALE 0.15
#define GHOST_SPEED 0.03

static const int ghosts_count = 4;

// board game:
GameBoard* board;

// mr. pacman
Pac* pacman;
bool pacFollowed = false;

// basic game information
std::string infoText[] =
{
	"Coins left: ",
	"Lives: ",
	"Game Over!"
};

// zooming - max Z distance and min Z distance
float maxZ = 15;
float minZ = 8;

// Camera up/down maximums:
float camUpDownMax = 0.8;

// Camera left/right maximums:
float camLRMax = 0.8;

// Camera rotation angles:
double theta = 0;
double phi = 0;

// Initial camera looking target
GLdouble centerX = GameBoard::CENTER_X;
GLdouble centerY = GameBoard::CENTER_Y;
GLdouble centerZ = GameBoard::CENTER_Z;
GLdouble centerDistance = 15;

// Camera (i.e. observer) coordinates
GLdouble eyeX = 0;
GLdouble eyeY = 0;
GLdouble eyeZ = 0;

// Lighting:
float lightIntensity = 1;
Ghost ghost_1(GHOST_SCALE, Vec3(0, 1, 0), 10, 1);
Ghost ghost_2(GHOST_SCALE, Vec3(1, 0, 0), 21, 5);
Ghost ghost_3(GHOST_SCALE, Vec3(0, 0, 1), 11, 9);
Ghost ghost_4(GHOST_SCALE, Vec3(1, 1, 1), 1, 1);

// Particles
ParticleSystem ps;
ParticleSystem ghost_1ps;
ParticleSystem ghost_2ps;
ParticleSystem ghost_3ps;
ParticleSystem ghost_4ps;
ParticleSystem energizerPS;


//Mocap
bool isMocap = false;

//Breakable walls 
const int wallCount = 5;
BreakableWall BWall_Array[wallCount];


// Breakable Wall Constructure signature --> new BreakableWall (float x, float y, float z, float sizeOfCube)

BreakableWall BWall1(12, 5, 0.0, 1);
BreakableWall BWall2(5, 13, 0.0, 1);
BreakableWall BWall3(17, 1, 0.0, 1);

BreakableWall BWall4(22, 13, 0.0, 1);
BreakableWall BWall5(5, 3, 0.0, 1);

float timeSinceStart = 0;
float oldTimeSinceStart = 0;

float ghostVulnerableTimer = 0.0f;
float ghostVulnerableTime = 9000.0f;

SoundManager* SoundManager::instance = NULL;

void init()
{
	glutSetWindow(1);

	ghost_1.setMoveSpeed(GHOST_SPEED);
	ghost_1.setMoveDirection(RIGHT);
	ghost_1.setPacman(pacman);
	ghost_1.setAIType(TRACKER);

	ghost_2.setMoveSpeed(GHOST_SPEED);
	ghost_2.setMoveDirection(DOWN);
	ghost_2.setPacman(pacman);
	ghost_2.setAIType(TRACKER);

	ghost_3.setMoveSpeed(GHOST_SPEED);
	ghost_3.setMoveDirection(LEFT);
	ghost_3.setPacman(pacman);
	ghost_3.setAIType(TRACKER);

	ghost_4.setMoveSpeed(GHOST_SPEED);
	ghost_4.setMoveDirection(DOWN);
	ghost_4.setPacman(pacman);
	ghost_4.setAIType(TRACKER);

	// General OpenGL configirations:
	glEnable(GL_DEPTH_TEST); // intialization of 3D rendering
	glEnable(GL_COLOR_MATERIAL); // object material properties enabled
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE); // set material properties which will be assigned by glColor
	glEnable(GL_NORMALIZE); // Automatically normalize normals (vectors of surfaces )

	glEnable(GL_LIGHTING); // general lighting enabled
	glEnable(GL_LIGHT0);

	// Create light components
	GLfloat ambientLight[] = { 0.2f, 0.2f, 0.2f, 1.0f };
	GLfloat diffuseLight[] = { 0.8f, 0.8f, 0.8, 1.0f };
	GLfloat specularLight[] = { 0.5f, 0.5f, 0.5f, 1.0f };
	GLfloat position[] = { GameBoard::CENTER_X, GameBoard::CENTER_Y, 4.0f, 1.0f };

	// Assign created components to GL_LIGHT0
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight);
	glLightfv(GL_LIGHT0, GL_POSITION, position);

	SoundManager::Instance()->loadSound(SoundEffects::Pacman_Chomp, "pacman_chomp.wav");
	SoundManager::Instance()->loadSound(SoundEffects::Pacman_EatGhost, "pacman_eatghost.wav");
	SoundManager::Instance()->loadSound(SoundEffects::Pacman_Hit, "pacman_hit.wav");
	SoundManager::Instance()->loadSound(SoundEffects::Pacman_PowerUp, "pacman_powerup.wav");
	SoundManager::Instance()->loadSound(SoundEffects::Pacman_Intro, "pacman_beginning.wav");

	SoundManager::Instance()->playSound(SoundEffects::Pacman_Intro);

	//glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 1.0);
	//glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.6);
	//glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.5);

	// Add global ambient light
	//GLfloat ambientColor[] = { 0.5, 0.5,  0.5, 1.0 };
	//glLightModelfv( GL_LIGHT_MODEL_AMBIENT, ambientColor); // ambient lights everywhere with the same amount 

	GLfloat mat_ambient[] = { 1.0, 1.0,  1.0, 1.0 };
	GLfloat mat_specular[] = { 1.0, 1.0,  1.0, 1.0 };

	//glMaterialfv( GL_FRONT, GL_AMBIENT, mat_ambient );
	//glMaterialfv( GL_FRONT, GL_SPECULAR, mat_specular );
	//glMaterialf( GL_FRONT_AND_BACK, GL_SHININESS, 10.0 );

	glShadeModel(GL_SMOOTH); // smooth shading
	glDepthFunc(GL_LESS);
}

void DrawInfo()
{
	// buffer for storing coinsCount
	static const int buf_length = 4;
	static char count_buffer[buf_length];
	std::sprintf(count_buffer, "%d", pacman->lives);
	std::sprintf(count_buffer + 1, "%d", board->coinsCount); // very unsafe! Be careful!

	// updating coins count
	// http://stackoverflow.com/questions/18847109/displaying-fixed-location-2d-text-in-a-3d-opengl-world-using-glut
	glDisable(GL_TEXTURE_2D);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, 500, 0, 500);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glColor3d(1.0, 1.0, 1.0);
	//std::cout << static_cast <char>(board->coinsCount) << std::endl;
	// text display functionality			
	for (int i = 0; i < sizeof(infoText) / sizeof(infoText[0]); i++)
	{
		glRasterPos2i(10, 500 - 20 - i * 18); // initial position of a raster.
		if (!pacman->isDead)
		{
			for (std::string::iterator j = infoText[i].begin(); j != infoText[i].end(); ++j)
			{
				if(i != 2)
					glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *j);
			}
		}
		else
		{
			for (std::string::iterator j = infoText[i].begin(); j != infoText[i].end(); ++j)
			{
				glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *j);
			}
		}
		if (i == 0)
		{   // display coinsCount
			for (int j = 1; j < buf_length; j++)
			{
				glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, count_buffer[j]);
			}
		}
		if (i == 1)
		{	// display lives
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, count_buffer[0]);
		}
	}
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	//added this
	glEnable(GL_TEXTURE_2D);
}

void display()
{
	timeSinceStart = float(glutGet(GLUT_ELAPSED_TIME));
	Time::deltaTime = timeSinceStart - oldTimeSinceStart;
	oldTimeSinceStart = timeSinceStart;


	glutSetWindow(1);
	glutPostRedisplay();

	//std::cout << "render" << std::endl;
	float orthoOffset = 2.0f;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	// TODO
	//glOrtho(-orthoOffset, GameBoard::DIM_X + orthoOffset, -orthoOffset, GameBoard::DIM_Y + orthoOffset, 0.1, 10 );
	//gluOrtho2D(-orthoOffset, GameBoard::DIM_X + orthoOffset, -orthoOffset, GameBoard::DIM_Y + orthoOffset );
	//glFrustum(-10.25, 10.25, -10.25, 10.25, 1, 1.3);
	gluPerspective(90, 1, 5, 30.0); 

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // czysc bufory

	// lighting stuff:
	glPushMatrix();
	GLfloat lightPos0[] = { pacman->x, pacman->y, 4.0f, 1.0f };
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos0);
	glPopMatrix();

	if (pacman->isPoweredUp)
	{
		ghost_1.setGhostColor(Vec3(0.5, 1.0, 1.0));
		ghost_2.setGhostColor(Vec3(0.5, 1.0, 1.0));
		ghost_3.setGhostColor(Vec3(0.5, 1.0, 1.0));
		ghost_4.setGhostColor(Vec3(0.5, 1.0, 1.0));

		ghostVulnerableTimer += Time::deltaTime;
		if (ghostVulnerableTimer >= ghostVulnerableTime)
		{
			pacman->isPoweredUp = false;
			ghostVulnerableTimer = 0;
			ghost_1.setOriginalGhostColor();
			ghost_2.setOriginalGhostColor();
			ghost_3.setOriginalGhostColor();
			ghost_4.setOriginalGhostColor();
		}
	}

	// move pacman
	if(!pacman->isDead)
		pacman->Move();

	// wall check stops movement
	pacman->WallCheck();

	ghost_1.collision(pacman);
	ghost_2.collision(pacman);
	ghost_3.collision(pacman);
	ghost_4.collision(pacman);

	ghost_1.display();
	//std::cout << ghost_1.getBoardPosition().f[0] << " " << ghost_1.getBoardPosition().f[1] << std::endl;
	ghost_2.display();
	ghost_3.display();
	ghost_4.display();

	if (pacFollowed)
	{ // look at pacman 
		centerX = pacman->x;
		centerY = pacman->y;
		centerZ = pacman->z;
	}
	else
	{ // look at the middle of the gameboard
		centerX = GameBoard::CENTER_X;
		centerY = GameBoard::CENTER_Y;
		centerZ = GameBoard::CENTER_Z;
	}

	eyeX = centerX + 5 * sin(theta);
	eyeY = centerY + centerDistance * sin(phi) + 5 * sin(theta);
	eyeZ = centerZ + centerDistance * cos(phi);
	gluLookAt(eyeX, eyeY, eyeZ, centerX, centerY, centerZ, sin(theta), cos(theta), 0);


	// coins eating
	if (pacman->Consume())
		board->coinsCount--;

	//Particles: Pacman
	if(!pacman->isDead)
		ps.addParticle(TrailParticle(Vec3(pacman->x, pacman->y, pacman->z), Vec3(0, 0, 0), Vec3(0, 0, 0), Vec3(0.7, 0.7, 0), 1, 255));

	//Particles: Ghosts
	ghost_1ps.addParticle(TrailParticle(Vec3(ghost_1.getBoardPosition().f[0], GameBoard::DIM_Y - ghost_1.getBoardPosition().f[1] - 1, 0), Vec3(0, 0, 0), Vec3(0, 0, 0), Vec3(0, 0.7, 0), 1.5, 255));
	ghost_2ps.addParticle(TrailParticle(Vec3(ghost_2.getBoardPosition().f[0], GameBoard::DIM_Y - ghost_2.getBoardPosition().f[1] - 1, 0), Vec3(0, 0, 0), Vec3(0, 0, 0), Vec3(0.7, 0, 0), 1.5, 255));
	ghost_3ps.addParticle(TrailParticle(Vec3(ghost_3.getBoardPosition().f[0], GameBoard::DIM_Y - ghost_3.getBoardPosition().f[1] - 1, 0), Vec3(0, 0, 0), Vec3(0, 0, 0), Vec3(0, 0, 0.7), 1.5, 255));
	ghost_4ps.addParticle(TrailParticle(Vec3(ghost_4.getBoardPosition().f[0], GameBoard::DIM_Y - ghost_4.getBoardPosition().f[1] - 1, 0), Vec3(0, 0, 0), Vec3(0, 0, 0), Vec3(0.7, 0.7, 0.7), 1.5, 255));

	//Particles: Energizer
	if (TrailParticle::isEnergizerPSActivated() || pacman->lives == 0) {
		//Add Particles

		srand(time(0));
		for (int i = 0; i < 100; i++) {

			float randVX = (rand() % 10 + 1) / 100.0;
			float randVY = (rand() % 10 + 1) / 100.0;
			float randVZ = (rand() % 10 + 1) / 100.0;

			float randAX = (rand() % 10 + 1) / 1000.0;
			float randAY = (rand() % 10 + 1) / 1000.0;
			float randAZ = (rand() % 10 + 1) / 1000.0;

			float randCX = (rand() % 10 + 1) / 10.0;
			float randCY = (rand() % 10 + 1) / 10.0;
			float randCZ = (rand() % 10 + 1) / 10.0;

			Vec3 randVel(randVX, randVY, randVZ);
			Vec3 randAcc(randAX, randAY, randAZ);
			Vec3 randColor(randCX, randCY, randCZ);
			energizerPS.addParticle(TrailParticle(Vec3(pacman->x, pacman->y, 0.0), randVel, randAcc, randColor, 1, 255));
		}

		TrailParticle::setEnergizerParticles(false);
	}

	//Displaying all Particles
	ps.run();
	ghost_1ps.run();
	ghost_2ps.run();
	ghost_3ps.run();
	ghost_4ps.run();
	energizerPS.run();

	// actual pacman, ghosts and board drawing
	board->Draw();
	pacman->Draw();


	// draw walls
	for (int i = 0; i < wallCount; i++)
	{
		if (!BWall_Array[i].GetIfExpired())
		{
			BWall_Array[i].Draw();
			// detect range to player
			BWall_Array[i].DistanceToPac();
		}
	}





	// screen information
	DrawInfo();

	glFlush(); // wyczysc wszystkie bufory. Standard zaleca wywolywanie tej komendy.
	glutSwapBuffers();
}


void reshape(GLsizei w, GLsizei h)
{
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	GLdouble aspect = w / (GLdouble)h;

	// rzutowanie perspektywiczne
	gluPerspective(90, 1, 5, 30.0);
}

void keyboard(unsigned char key, int x, int y)
{
	static float step = 0.1;
	static float stepZoom = 1;

	// Camera / zooming
	switch (key)
	{
	case 'a':
		pacman->Turn(180);
		break;

	case 'w':
		// up
		pacman->Turn(90);
		break;

	case 'd':
		pacman->Turn(0);
		break;

	case 's':
		pacman->Turn(270);
		break;

	case 'k':
		if (isMocap) {
			glutSetWindow(2);
			glutHideWindow();
			isMocap = false;
		}
		else {
			glutSetWindow(2);
			glutShowWindow();
			isMocap = true;
		}
		break;
	case 27:
		exit(0);
		break;
	}
}

// Mouse handling
void mouse(int button, int state, int x, int y)
{
	if (button == GLUT_RIGHT_BUTTON) {
		exit(0);
	}
}

// Special keys handling (arrows)
void special(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_LEFT:
		pacman->Turn(180);
		break;

	case GLUT_KEY_RIGHT:
		pacman->Turn(0);
		break;

	case GLUT_KEY_UP:
		pacman->Turn(90);
		break;

	case GLUT_KEY_DOWN:
		pacman->Turn(270);
		break;
	}


	reshape(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
}

void timer(int v)
{
	// maybe some visual effects?
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH); // pojedyncze buforowanie oraz bufor glebokosci ustawiamy
	glutInitWindowPosition(100, 10);
	glutInitWindowSize(1000, 1000);
	glutCreateWindow("PackMan");

	//glutGameModeString( "800x600:16@60" );
	//glutEnterGameMode();

	glutSetCursor(GLUT_CURSOR_NONE);

	glutDisplayFunc(display);
	glutReshapeFunc(reshape); // trzeba zmienic parametry rzutowania
	glutIdleFunc(display); // scena jest caly czas przeliczana w tle

	glutTimerFunc(40, timer, 1);

	glutSpecialFunc(special);
	glutKeyboardFunc(keyboard);

	// OpenGL objects initialization:
	pacman = new Pac(15, 2);
	board = new GameBoard();


	//attach pacman reference to walls
	BWall1.AttachPacmanReference(pacman);
	BWall2.AttachPacmanReference(pacman);
	BWall3.AttachPacmanReference(pacman);
	BWall4.AttachPacmanReference(pacman);
	BWall5.AttachPacmanReference(pacman);


	// init walls
	BWall_Array[0] = BWall1;
	BWall_Array[1] = BWall2;
	BWall_Array[2] = BWall3;
	BWall_Array[3] = BWall4;
	BWall_Array[4] = BWall5;




	init();

	//kinect initialization
	launchKinect(argc, argv, pacman);

	glutMainLoop(); // w momencie wywolania nasz program zaczyna dzialac. Reaguje od tego momentu na zdarzenia.

	return 0;
}