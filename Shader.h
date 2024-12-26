#pragma once

#include "Core.h"

class Shader {
protected:
	char* vertexShaderSource;
	char* fragmentShaderSource;
	char* geometryShaderSource;
	char* computeShaderSource;

	std::unordered_map<std::string, GLuint> shaderUniforms{};

	std::string vertFileName, fragFileName, geomFileName, compFileName;
	GLuint vertexShaderID, fragmentShaderID, geometryShaderID, computeShaderID, programID;

	void checkUniform(const char* varName) {
		if(this->shaderUniforms.find(varName) == this->shaderUniforms.end())
			this->shaderUniforms[varName] = glGetUniformLocation(this->programID, varName);
	}

	void readFile(std::string fileName, char*& shader);
	void compileShader(GLuint& shaderID, const char* shader, GLenum shaderType) const;
	void attachShader(std::vector<GLuint> shaderIDs);

public:
	Shader(std::string compFileName = "");
	Shader(std::string vertFileName, std::string fragFileName, std::string geomFileName = "");

	void useShader() const { glUseProgram(this->programID); }
	void endShader() const { glUseProgram(0); }
	void dispatchComputeShader(int x, int y, int z, GLbitfield barriers) const {
		glDispatchCompute(x, y, z);
		glMemoryBarrier(barriers);
	}

	std::unordered_map<std::string, GLuint> getShaderUniforms() {
		return this->shaderUniforms;
	}

	void setInt(const char* varName, const int& value) {
		this->checkUniform(varName);
		glUniform1i(this->shaderUniforms[varName], value);
	}

	void setUint(const char* varName, const uint& value) {
		this->checkUniform(varName);
		glUniform1ui(this->shaderUniforms[varName], value);
	}

	void setFloat(const char* varName, const float& value) {
		this->checkUniform(varName);
		glUniform1f(this->shaderUniforms[varName], value);
	}

	void setVec1(const char* varName, const glm::vec1& value) {
		this->checkUniform(varName);
		glUniform1fv(this->shaderUniforms[varName], 1, glm::value_ptr(value));
	}

	void setiVec1(const char* varName, const glm::ivec1& value) {
		this->checkUniform(varName);
		glUniform1iv(this->shaderUniforms[varName], 1, glm::value_ptr(value));
	}

	void setVec2(const char* varName, const glm::vec2& value) {
		this->checkUniform(varName);
		glUniform2fv(this->shaderUniforms[varName], 1, glm::value_ptr(value));
	}

	void setiVec2(const char* varName, const glm::ivec2& value) {
		this->checkUniform(varName);
		glUniform2iv(this->shaderUniforms[varName], 1, glm::value_ptr(value));
	}

	void setVec3(const char* varName, const glm::vec3& value) {
		this->checkUniform(varName);
		glUniform3fv(this->shaderUniforms[varName], 1, glm::value_ptr(value));
	}

	void setiVec3(const char* varName, const glm::ivec3& value) {
		this->checkUniform(varName);
		glUniform3iv(this->shaderUniforms[varName], 1, glm::value_ptr(value));
	}

	void setVec4(const char* varName, const glm::vec4& value) {
		this->checkUniform(varName);
		glUniform4fv(this->shaderUniforms[varName], 1, glm::value_ptr(value));
	}

	void setiVec4(const char* varName, const glm::ivec4& value) {
		this->checkUniform(varName);
		glUniform4iv(this->shaderUniforms[varName], 1, glm::value_ptr(value));
	}

	void setMat2(const char* varName, const glm::mat2& value) {
		this->checkUniform(varName);
		glUniformMatrix2fv(this->shaderUniforms[varName], 1, GL_FALSE, glm::value_ptr(value));
	}

	void setMat3(const char* varName, const glm::mat3& value) {
		this->checkUniform(varName);
		glUniformMatrix3fv(this->shaderUniforms[varName], 1, GL_FALSE, glm::value_ptr(value));
	}

	void setMat4(const char* varName, const glm::mat4& value) {
		this->checkUniform(varName);
		glUniformMatrix4fv(this->shaderUniforms[varName], 1, GL_FALSE, glm::value_ptr(value));
	}

	void setTexture(const char* samplerName, const GLuint64& handle) {
		this->checkUniform(samplerName);
		glUniformHandleui64ARB(this->shaderUniforms[samplerName], handle);
	}

	~Shader();
};