#include "TrailParticle.h"
#include "GLUT.H"

bool TrailParticle::activateEnergizerParticles;

TrailParticle::TrailParticle()
{
}

TrailParticle::TrailParticle(Vec3 location, Vec3 velocity, Vec3 acceleration, Vec3 color,float alpha ,float lifespan)
{
	this->location = location;
	this->velocity = velocity;
	this->acceleration = acceleration;
	this->color = color;
	this->alpha = alpha;
	this->lifespan = lifespan;
}

void TrailParticle::update()
{
	velocity += acceleration;
	location += velocity;

	//lifespan -= 10;
	alpha -= 0.025;
}

void TrailParticle::display()
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glPushMatrix();
	glTranslatef(location.f[0], location.f[1], location.f[2]);
	glColor4f(color.f[0], color.f[1], color.f[2], alpha); 
	glutSolidCube(0.15);
	glPopMatrix();
}

bool TrailParticle::isDead()
{
	if (lifespan <= 0 || alpha <= 0)
		return true;
	else
		return false;
}

void TrailParticle::setEnergizerParticles(bool set)
{
	activateEnergizerParticles = set;
}

bool TrailParticle::isEnergizerPSActivated()
{
	return activateEnergizerParticles;
}
