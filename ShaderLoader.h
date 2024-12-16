#pragma once

#include "Core.h"

class IShaderLoader {
protected:
	char* vertexShader;
	char* fragmentShader;
	char* geometryShader;

	std::string vertFileName, fragFileName, geomFileName;
	GLuint vertexShaderID, fragmentShaderID, geometryShaderID, programID;

public:
	IShaderLoader(std::string vertFileName = "", std::string fragFileName = "", std::string geomFileName = "");

	virtual void readFile(std::string fileName, char*& shader);
	virtual void compileShader(GLuint& shaderID, const char* shader, GLenum shaderType);
	virtual void attachShader(std::vector<GLuint> shaderIDs);
	virtual void getUniformLocations() = 0;

	virtual inline GLuint getProgramID() const { return this->programID; };

	~IShaderLoader();
};

