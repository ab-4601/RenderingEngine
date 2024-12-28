#include "Particle.h"

CParticle::CParticle(vec3 position, vec3 velocity, vec3 color, float forceEffect, float rotation,
	float scale, float lifeTime)
	: position { position }, velocity{ velocity }, color{ color }, forceEffect{ forceEffect },
	rotation{ rotation }, scale{ scale }, lifeTime{ lifeTime }, 
	distance{ 0.f }, blend{ 0.f }, texOffset1{ {} }, texOffset2{ {} } {

}

void CParticle::updateTextureCoordInfo() {
	float lifeFactor = elapsedTime / lifeTime;
}

bool CParticle::update(glm::vec3 cameraPosition) {
	float angle = (float)(rand() % 360);
	velocity.y += GRAVITY * forceEffect * delta;

	vec3 change = velocity;
	change *= delta;

	position = change + position;
	distance = glm::distance(cameraPosition, position);

	elapsedTime += delta;

	return elapsedTime < lifeTime;
}