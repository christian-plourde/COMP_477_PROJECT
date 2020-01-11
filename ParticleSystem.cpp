#include "ParticleSystem.h"

void ParticleSystem::addParticle(TrailParticle p)
{
	particles.push_back(p);
}

void ParticleSystem::run()
{
	for (int i = particles.size() - 1; i >= 0; i--) {
		particles[i].update();
		particles[i].display();

		if (particles[i].isDead()) {
			particles.erase(particles.begin() + i);
			particles.shrink_to_fit();
		}
	}
}

int ParticleSystem::getSize()
{
	return particles.size();
}
