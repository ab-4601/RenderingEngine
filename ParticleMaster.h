#pragma once

#include "Particle.h"
#include "ParticleTexture.h"
#include "ParticleRenderer.h"

class ParticleMaster {
private:
	friend CParticle;
	std::vector<CParticle*> particles;
	ParticleRenderer renderer;

	ParticleTexture texture;

public:
	ParticleMaster(ParticleTexture texture = ParticleTexture("Textures/cosmic.png", 4.f)) {
		this->renderer.setTexture(texture);
		this->particles = {};
	}

	ParticleMaster(const ParticleMaster& source) noexcept;

	ParticleMaster(ParticleMaster&& source) noexcept;

	inline void setParticleTexture(const ParticleTexture& texture) { 
		this->texture = texture;
		this->renderer.setTexture(this->texture);
	}

	void update(float delta, glm::vec3 cameraPosition);

	void renderParticles(const Window* const currWindow, const Camera& camera,
		glm::mat4& model, const glm::mat4& projection) {
		this->renderer.render(this->particles, currWindow, camera, model, projection);
	}

	void generateParticle(vec3 position, vec3 velocity, vec3 color, float forceEffect, float rotation,
		float scale, float lifeTime);

	~ParticleMaster() {
		for (auto& particle : this->particles)
			delete particle;
	}
};

