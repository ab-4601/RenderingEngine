#pragma once

#include "Core.h"
#include "Particle.h"
#include "ParticleTexture.h"
#include "ParticleMaster.h"
#include <time.h>

class ParticleSystem {
private:
	int pps;
	float gravityComplient, lifeLength;
	float scale;
	glm::vec3 color;

	ParticleMaster master;
	ParticleTexture texture;

public:
	ParticleSystem(glm::vec3 color = { 1.f, 0.5f, 0.05f }, int pps = 10, 
		float gravityComplient = 1.f, float lifeLength = 5.f, float scale = 1.f, 
		ParticleTexture texture = ParticleTexture("Textures/cosmic.png", 4.f));

	void generateParticles(glm::vec3 &systemCenter, float rotationAngle = 0.f);

	void renderParticles(const Window* const currWindow, const Camera& camera, glm::mat4& model) {
		this->master.renderParticles(currWindow, camera, model, camera.getProjectionMatrix());
	}

	void updateParticles(float delta, glm::vec3 cameraPosition) {
		this->master.update(delta, cameraPosition);
	}

	~ParticleSystem() = default;
};

