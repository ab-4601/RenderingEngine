#pragma once

#include "Core.h"

class PerlinNoise {
private:
	float persistence, frequency, amplitude;
	int octaves, randomSeed;

public:
	PerlinNoise(float persistence = 0.f, float frequency = 0.f, float amplitude = 0.f, int octaves = 0, int randomSeed = 0);

	float Total(float i, float j) const;
	float GetValue(float x, float y) const;
	float Interpolate(float x, float y, float a) const;
	float Noise(int x, int y) const;

	inline float getHeight(float x, float y) const {
		return this->amplitude * this->Total(x, y);
	}

	inline float getAmplitude() const { return this->amplitude; }
	inline float getFrequency() const { return this->frequency; }
	inline float getPersistence() const { return this->persistence; }
	inline int getOctaves() const { return this->octaves; }
	inline int getRandomSeed() const { return this->randomSeed; }

	inline void setPersistence(float _persistence) {
		this->persistence = _persistence;
	}

	inline void setFrequency(float _frequency) {
		this->frequency = _frequency;
	}

	inline void setAmplitude(float _amplitude) {
		this->amplitude = _amplitude;
	}

	inline void setOctaves(int _octaves) {
		this->octaves = _octaves;
	}

	inline void setRandomSeed(int _randomSeed) {
		this->randomSeed = _randomSeed;
	}

	~PerlinNoise() = default;
};

