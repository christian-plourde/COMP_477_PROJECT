/*
make:
constructor --> initialize with size, and coordinate location.
destroy function. (bool if destroyed --> if yes then dont draw it.

breaking part --> do it like a particle system, spawns smaller wall models (cubes of same sizes which fall around.)

if possible, lets do a wall texture.
*/

#include"BreakableWall.h"

BreakableWall::BreakableWall()
{

}

BreakableWall::BreakableWall(float xPos, float yPos, float zPos, float wallSize)
{
	xPosition = xPos;
	yPosition = yPos;
	zPosition = zPos;
	size = wallSize;
	isDestroyed = false;
}

void BreakableWall::Draw()
{
	// only draw if it is not destroyed.


	if (!GetDestroyed())
	{
		//draw wall at location
		glNormal3f(0.0f, 0.0f, 1.0f);
		glColor3f(1, 0, 0); // Red
		glPushMatrix();
		glTranslatef(xPosition, yPosition, 0);
		glutSolidCube(size);
		glPopMatrix();
	}

	// draw destroyed particles now:
	
	if (GetDestroyed() && !expired) 
	{
		movementOffset += 0.05;
		rotationOffset += 5;
		if (!particleFall)
		{
			zOffset += 0.02;
			if (zOffset > 0.3)
			{
				particleFall = true;
				fallMultipler = 2;
			}
		}
		else
		{
			zOffset -= 0.04;
		}

		// block piece 1
		//draw wall at location
		glNormal3f(0.0f, 0.0f, 1.0f);
		glColor3f(1, 0, 0); // red
		glPushMatrix();
		glTranslatef(xPosition+movementOffset* speedMultiplier, yPosition+movementOffset*speedMultiplier, 0+zOffset* fallMultipler);
		//rotation
		glRotatef(10+rotationOffset,5,5,5);

		
		// no fallthrough effect
		//glTranslatef(xPosition+movementOffset* speedMultiplier, yPosition+movementOffset*speedMultiplier, 0);
		glutSolidCube(size*0.5f);
		glPopMatrix();


		//block piece 2
		//draw wall at location
		glNormal3f(0.0f, 0.0f, 1.0f);
		glColor3f(1, 0, 0); // red
		glPushMatrix();
		glTranslatef(xPosition-movementOffset* speedMultiplier, yPosition-movementOffset* speedMultiplier, 0 + zOffset * fallMultipler);
		//rotation
		glRotatef(10 - rotationOffset, 5, 5, 5);
		glutSolidCube(size*0.5f);
		glPopMatrix();

		// block piece 3
		//draw wall at location
		glNormal3f(0.0f, 0.0f, 1.0f);
		glColor3f(1, 0, 0); // red
		glPushMatrix();
		glTranslatef(xPosition - movementOffset* speedMultiplier, yPosition + movementOffset* speedMultiplier, 0 + zOffset * fallMultipler);
		//rotation
		glRotatef(10 + rotationOffset, 5, 5, 5);
		glutSolidCube(size*0.5f);
		glPopMatrix();


		// block piece 4
		//draw wall at location
		glNormal3f(0.0f, 0.0f, 1.0f);
		glColor3f(1, 0, 0); // red
		glPushMatrix();
		glTranslatef(xPosition + movementOffset* speedMultiplier, yPosition - movementOffset* speedMultiplier, 0 + zOffset * fallMultipler);
		//rotation
		glRotatef(10 - rotationOffset, 5, 5, 5);
		glutSolidCube(size*0.5f);
		glPopMatrix();


		// block piece 5
		//draw wall at location
		glNormal3f(0.0f, 0.0f, 1.0f);
		glColor3f(1, 0, 0); // red
		glPushMatrix();
		glTranslatef(xPosition + movementOffset* speedMultiplier, yPosition, 0 + zOffset * fallMultipler);
		//rotation
		glRotatef(10 + rotationOffset, 5, 5, 5);
		glutSolidCube(size*0.5f);
		glPopMatrix();

		// block piece 6
		//draw wall at location
		glNormal3f(0.0f, 0.0f, 1.0f);
		glColor3f(1, 0, 0); // red
		glPushMatrix();
		glTranslatef(xPosition - movementOffset* speedMultiplier, yPosition, 0 + zOffset * fallMultipler);
		//rotation
		glRotatef(10 - rotationOffset, 5, 5, 5);
		glutSolidCube(size*0.5f);
		glPopMatrix();


		// block piece 7
		//draw wall at location
		glNormal3f(0.0f, 0.0f, 1.0f);
		glColor3f(1, 0, 0); // red
		glPushMatrix();
		glTranslatef(xPosition, yPosition + movementOffset* speedMultiplier, 0 + zOffset * fallMultipler);
		//rotation
		glRotatef(10 + rotationOffset, 5, 5, 5);
		glutSolidCube(size*0.5f);
		glPopMatrix();

		// block piece 8
		//draw wall at location
		glNormal3f(0.0f, 0.0f, 1.0f);
		glColor3f(1, 0, 0); // red
		glPushMatrix();
		glTranslatef(xPosition, yPosition - movementOffset* speedMultiplier, 0 + zOffset * fallMultipler);
		//rotation
		glRotatef(10 - rotationOffset, 5, 5, 5);
		glutSolidCube(size*0.5f);
		glPopMatrix();





		// delete 
		if (movementOffset > 3.5)
			expired = true;
	}
	
}


void BreakableWall::DistanceToPac()
{
	if (pacmanRef!=NULL)
	{
		if((int)(pacmanRef->x) == (int)(xPosition) && (int)(pacmanRef->y) ==(int)(yPosition))
		{
			// destory and stop drawing.
			SetDestroyWall();
		}
	}
}