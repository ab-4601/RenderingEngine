#include "ParticleTexture.h"

ParticleTexture::ParticleTexture(const ParticleTexture& source)
	: Texture(source.fileLocation), numOfRows{ source.numOfRows } {

}

ParticleTexture::ParticleTexture(ParticleTexture&& source) noexcept
	: Texture(source.fileLocation), numOfRows{ source.numOfRows } {

	source.~ParticleTexture();
}

ParticleTexture& ParticleTexture::operator=(const ParticleTexture& rhs) {
	if (this == &rhs)
		return *this;

	this->fileLocation = rhs.fileLocation;
	this->numOfRows = rhs.numOfRows;

	return *this;
}

ParticleTexture& ParticleTexture::operator=(ParticleTexture&& rhs) noexcept {
	if (this == &rhs)
		return *this;

	this->fileLocation = rhs.fileLocation;
	this->numOfRows = rhs.numOfRows;

	rhs.~ParticleTexture();

	return *this;
}