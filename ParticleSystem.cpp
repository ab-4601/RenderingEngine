#include "ParticleSystem.h"

ParticleSystem::ParticleSystem(glm::vec3 color, int pps, float gravityComplient,
	float lifeLength, float scale, ParticleTexture texture)
	: color{ color }, pps{ pps }, gravityComplient{ gravityComplient }, lifeLength{ lifeLength }, scale{ scale },
	texture{ texture } {

	master.setParticleTexture(this->texture);
}

void ParticleSystem::generateParticles(glm::vec3& systemCenter, float rotationAngle) {
	//systemCenter.x = 50 * cosf(50 * rotationAngle);
	//systemCenter.y = 50 * sinf(50 * rotationAngle);

	for (int i = 0; i < this->pps; i++) {
		float x = static_cast<float>(rand() % 100 - 50);
		//x = 50 * cosf(glm::radians(rotationAngle));

		float y = static_cast<float>(rand() % 100 - 50);

		float z = static_cast<float>(rand() % 100 - 50);
		//z = 50 * sinf(glm::radians(rotationAngle));

		glm::vec3 velocity{ x, y, z };

		master.generateParticle(systemCenter, velocity, color, gravityComplient,
			0.f, scale, lifeLength);
	}
}