#include "Particle.h"

CParticle::CParticle(vec3 position, vec3 velocity, vec3 color, float forceEffect, float rotation,
	float scale, float lifeTime)
	: position { position }, velocity{ velocity }, color{ color }, forceEffect{ forceEffect },
	rotation{ rotation }, scale{ scale }, lifeTime{ lifeTime }, 
	distance{ 0.f }, blend{ 0.f }, texOffset1{ {} }, texOffset2{ {} } {

}

void CParticle::updateTextureCoordInfo() {
	float lifeFactor = this->elapsedTime / this->lifeTime;
}

bool CParticle::update(glm::vec3 cameraPosition) {
	float angle = (float)(rand() % 360);
	this->velocity.y += GRAVITY * this->forceEffect * this->delta;

	vec3 change = this->velocity;
	change *= this->delta;

	this->position = change + position;
	this->distance = glm::distance(cameraPosition, this->position);

	this->elapsedTime += delta;

	return this->elapsedTime < this->lifeTime;
}