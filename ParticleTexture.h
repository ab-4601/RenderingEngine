#pragma once

#include "Core.h"
#include "Texture.h"

class ParticleTexture : public Texture {
private:
	float numOfRows;

public:
	ParticleTexture(std::string fileLocation = "", float numOfRows = 4.f)
		: Texture(fileLocation), numOfRows{ numOfRows } {}

	ParticleTexture(const ParticleTexture& source);
	ParticleTexture(ParticleTexture&& source) noexcept;

	ParticleTexture& operator=(const ParticleTexture& rhs);
	ParticleTexture& operator=(ParticleTexture&& rhs) noexcept;

	inline GLuint getTextureID() const { return this->textureID; }
	inline float getNumberOfRows() const { return this->numOfRows; }

	~ParticleTexture() = default;
};

