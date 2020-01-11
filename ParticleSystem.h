#pragma once

#include <vector>
#include "TrailParticle.h"

class ParticleSystem {
	std::vector<TrailParticle> particles;

public:

	void addParticle(TrailParticle p);
	void run();

	int getSize();
};
