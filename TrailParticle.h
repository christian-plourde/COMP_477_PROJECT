#pragma once

#include "Vec3.h"

class TrailParticle {
	Vec3 location;
	Vec3 velocity;
	Vec3 acceleration;
	Vec3 color;
	float alpha;
	float lifespan;

	static bool activateEnergizerParticles;

public:

	TrailParticle();

	TrailParticle(Vec3 location, Vec3 velocity, Vec3 acceleration, Vec3 color, float alpha ,float lifespan);

	void update();

	void display();

	bool isDead();

	static void setEnergizerParticles(bool set);

	static bool isEnergizerPSActivated();
};
