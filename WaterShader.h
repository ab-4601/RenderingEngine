#pragma once

#include "ShaderLoader.h"

class WaterShader : public IShaderLoader {
	GLuint uniformModel, uniformProjection, uniformView;

public:
	WaterShader(std::string vertFilename = "water.vert", std::string fragFileName = "water.frag");

	virtual void getUniformLocations() override;

	inline GLuint getUniformModel() const { return this->uniformModel; }
	inline GLuint getUniformProjection() const { return this->uniformProjection; }
	inline GLuint getUniformView() const { return this->uniformView; }

	~WaterShader() = default;
};

