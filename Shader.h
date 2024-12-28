#pragma once

#include "Core.h"

class Shader {
protected:
	char* vertexShaderSource;
	char* fragmentShaderSource;
	char* geometryShaderSource;
	char* computeShaderSource;

	std::unordered_map<std::string, GLuint> shaderUniforms{};

	GLuint vertexShaderID, fragmentShaderID, geometryShaderID, computeShaderID, programID;

	void checkUniform(const char* varName) {
		if(shaderUniforms.find(varName) == shaderUniforms.end())
			shaderUniforms[varName] = glGetUniformLocation(programID, varName);
	}

	void readFile(std::string fileName, char*& shader);
	void compileShader(std::string fileName, GLuint& shaderID, const char* shader, GLenum shaderType) const;
	void attachShader(std::vector<std::string> files, std::vector<GLuint> shaderIDs);

public:
	Shader(std::string compFileName = "");
	Shader(std::string vertFileName, std::string fragFileName, std::string geomFileName = "");

	void useShader() const { glUseProgram(programID); }
	void endShader() const { glUseProgram(0); }
	void dispatchComputeShader(int x, int y, int z, GLbitfield barriers) const {
		glDispatchCompute(x, y, z);
		glMemoryBarrier(barriers);
	}

	std::unordered_map<std::string, GLuint> getShaderUniforms() {
		return shaderUniforms;
	}

	void setInt(const char* varName, const int& value) {
		checkUniform(varName);
		glUniform1i(shaderUniforms[varName], value);
	}

	void setUint(const char* varName, const uint& value) {
		checkUniform(varName);
		glUniform1ui(shaderUniforms[varName], value);
	}

	void setFloat(const char* varName, const float& value) {
		checkUniform(varName);
		glUniform1f(shaderUniforms[varName], value);
	}

	void setVec1(const char* varName, const glm::vec1& value) {
		checkUniform(varName);
		glUniform1fv(shaderUniforms[varName], 1, glm::value_ptr(value));
	}

	void setiVec1(const char* varName, const glm::ivec1& value) {
		checkUniform(varName);
		glUniform1iv(shaderUniforms[varName], 1, glm::value_ptr(value));
	}

	void setVec2(const char* varName, const glm::vec2& value) {
		checkUniform(varName);
		glUniform2fv(shaderUniforms[varName], 1, glm::value_ptr(value));
	}

	void setiVec2(const char* varName, const glm::ivec2& value) {
		checkUniform(varName);
		glUniform2iv(shaderUniforms[varName], 1, glm::value_ptr(value));
	}

	void setVec3(const char* varName, const glm::vec3& value) {
		checkUniform(varName);
		glUniform3fv(shaderUniforms[varName], 1, glm::value_ptr(value));
	}

	void setiVec3(const char* varName, const glm::ivec3& value) {
		checkUniform(varName);
		glUniform3iv(this->shaderUniforms[varName], 1, glm::value_ptr(value));
	}

	void setVec4(const char* varName, const glm::vec4& value) {
		checkUniform(varName);
		glUniform4fv(shaderUniforms[varName], 1, glm::value_ptr(value));
	}

	void setiVec4(const char* varName, const glm::ivec4& value) {
		checkUniform(varName);
		glUniform4iv(shaderUniforms[varName], 1, glm::value_ptr(value));
	}

	void setMat2(const char* varName, const glm::mat2& value) {
		checkUniform(varName);
		glUniformMatrix2fv(shaderUniforms[varName], 1, GL_FALSE, glm::value_ptr(value));
	}

	void setMat3(const char* varName, const glm::mat3& value) {
		checkUniform(varName);
		glUniformMatrix3fv(shaderUniforms[varName], 1, GL_FALSE, glm::value_ptr(value));
	}

	void setMat4(const char* varName, const glm::mat4& value) {
		checkUniform(varName);
		glUniformMatrix4fv(shaderUniforms[varName], 1, GL_FALSE, glm::value_ptr(value));
	}

	void setTexture(const char* samplerName, const GLuint64& handle) {
		checkUniform(samplerName);
		glUniformHandleui64ARB(shaderUniforms[samplerName], handle);
	}

	void bindTexture(GLenum textureLocation, GLenum textureType, uint textureID) {
		glActiveTexture(textureLocation);
		glBindTexture(textureType, textureID);
	}

	~Shader();
};