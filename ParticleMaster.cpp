#include "ParticleMaster.h"

ParticleMaster::ParticleMaster(const ParticleMaster& source) noexcept {
	CParticle* sourceParticle = nullptr;
	this->particles.clear();

	for (int i = 0; i < source.particles.size(); i++) {
		sourceParticle = source.particles.at(i);

		CParticle* currParticle = new CParticle(sourceParticle->getPosition(), sourceParticle->getVelocity(),
			sourceParticle->getColor(), sourceParticle->getForceEffect(), sourceParticle->getRotation(),
			sourceParticle->getScale(), sourceParticle->getLifeTime());

		this->particles.push_back(currParticle);
	}
}

ParticleMaster::ParticleMaster(ParticleMaster&& source) noexcept {
	CParticle* sourceParticle = nullptr;
	this->particles.clear();

	for (int i = 0; i < source.particles.size(); i++) {
		sourceParticle = source.particles.at(i);

		CParticle* currParticle = new CParticle(sourceParticle->getPosition(), sourceParticle->getVelocity(),
			sourceParticle->getColor(), sourceParticle->getForceEffect(), sourceParticle->getRotation(),
			sourceParticle->getScale(), sourceParticle->getLifeTime());

		this->particles.push_back(currParticle);
	}

	source.~ParticleMaster();
}

void ParticleMaster::update(float delta, glm::vec3 cameraPosition) {
	CParticle* particle = nullptr;

	for (size_t i = 0; i < this->particles.size(); i++) {
		particle = this->particles.at(i);
		particle->updateDelta(delta);
		bool stillVisible = particle->update(cameraPosition);

		if (!stillVisible) {
			auto iterator = this->particles.begin() + i;
			this->particles.erase(iterator);
			delete particle;
		}
	}
}

void ParticleMaster::generateParticle(vec3 position, vec3 velocity, vec3 color, float forceEffect, 
	float rotation, float scale, float lifeTime) {

	CParticle* particle = new CParticle(position, velocity, color, forceEffect,
		rotation, scale, lifeTime);
	this->particles.push_back(particle);

	/*auto compare = [](CParticle* a, CParticle* b) -> bool {
		return a->getDistance() < b->getDistance();
	};

	std::sort(this->particles.begin(), this->particles.end(), compare);*/
}