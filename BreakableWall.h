#pragma once


#include "Vec3.h"
#include <GL/glew.h>
#include "glut.h"


//#include "pac.h"
#include "creature.h"
//#include "ghost.h"

#define TIME_STEPSIZE2 0.5*0.5 // how large time step each particle takes each frame

class BreakableWall
{
private:
	//position variables:
	float xPosition;
	float yPosition;
	float zPosition;
	

	float size;

	bool isDestroyed;
	bool expired;
	float movementOffset = 0.0;         // how the particles float away (simple version)

	Creature *pacmanRef;
	//Pac *pacmanRef;
	float distanceToPacMan;


	//experimental
	//fall in wall parts
	float zOffset = 0;
	bool particleFall = false;

	float speedMultiplier = 1.2;
	float fallMultipler = 2.5;

	float rotationOffset = 10;


public:
	//BreakableWall(Vec3 spawnLocation, float wallSize );
	BreakableWall();
	BreakableWall(float xPos, float yPos, float zPos, float wallSize );

	void Draw();			//; render the wall
	void DistanceToPac();
	void Destroy();      // do the destruction particle effect and stop rendering.


	void AttachPacmanReference(Creature *pacRef)
	{
		if (pacRef != NULL)
		{
			pacmanRef = pacRef;
		}
	}

	bool GetDestroyed()
	{
		return isDestroyed;
	}

	void SetDestroyWall()
	{
		isDestroyed = true;
	}

	bool GetIfExpired()
	{
		return expired;
	}
};