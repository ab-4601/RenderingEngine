#pragma once

#include <typeinfo>
#include "Core.h"

class Shader {
protected:
	char* vertexShader;
	char* fragmentShader;
	char* geometryShader;
	char* computeShader;

	std::unordered_map<const char*, GLuint> shaderUniforms{};

	std::string vertFileName, fragFileName, geomFileName, compFileName;
	GLuint vertexShaderID, fragmentShaderID, geometryShaderID, computeShaderID, programID;

	inline void checkUniform(const char* varName) {
		if(this->shaderUniforms.find(varName) == this->shaderUniforms.end())
			this->shaderUniforms[varName] = glGetUniformLocation(this->programID, varName);
	}

public:
	Shader(std::string vertFileName = "", std::string fragFileName = "", std::string geomFileName = "");
	Shader(std::string compFileName = "");

	void readFile(std::string fileName, char*& shader);
	void compileShader(GLuint& shaderID, const char* shader, GLenum shaderType);
	void attachShader(std::vector<GLuint> shaderIDs);

	inline void useShader() const { glUseProgram(this->programID); }
	inline void endShader() const { glUseProgram(0); }

	inline void setInt(const char* varName, const int& value) {
		this->checkUniform(varName);
		glUniform1i(this->shaderUniforms[varName], value);
	}

	inline void setUint(const char* varName, const uint& value) {
		this->checkUniform(varName);
		glUniform1ui(this->shaderUniforms[varName], value);
	}

	inline void setFloat(const char* varName, const float& value) {
		this->checkUniform(varName);
		glUniform1f(this->shaderUniforms[varName], value);
	}

	inline void setVec1(const char* varName, const glm::vec1& value) {
		this->checkUniform(varName);
		glUniform1fv(this->shaderUniforms[varName], 1, glm::value_ptr(value));
	}

	inline void setiVec1(const char* varName, const glm::ivec1& value) {
		this->checkUniform(varName);
		glUniform1iv(this->shaderUniforms[varName], 1, glm::value_ptr(value));
	}

	inline void setVec2(const char* varName, const glm::vec2& value) {
		this->checkUniform(varName);
		glUniform2fv(this->shaderUniforms[varName], 1, glm::value_ptr(value));
	}

	inline void setiVec2(const char* varName, const glm::ivec2& value) {
		this->checkUniform(varName);
		glUniform2iv(this->shaderUniforms[varName], 1, glm::value_ptr(value));
	}

	inline void setVec3(const char* varName, const glm::vec3& value) {
		this->checkUniform(varName);
		glUniform3fv(this->shaderUniforms[varName], 1, glm::value_ptr(value));
	}

	inline void setiVec3(const char* varName, const glm::ivec3& value) {
		this->checkUniform(varName);
		glUniform3iv(this->shaderUniforms[varName], 1, glm::value_ptr(value));
	}

	inline void setVec4(const char* varName, const glm::vec4& value) {
		this->checkUniform(varName);
		glUniform4fv(this->shaderUniforms[varName], 1, glm::value_ptr(value));
	}

	inline void setiVec4(const char* varName, const glm::ivec4& value) {
		this->checkUniform(varName);
		glUniform4iv(this->shaderUniforms[varName], 1, glm::value_ptr(value));
	}

	inline void setMat2(const char* varName, const glm::mat2& value) {
		this->checkUniform(varName);
		glUniformMatrix2fv(this->shaderUniforms[varName], 1, GL_FALSE, glm::value_ptr(value));
	}

	inline void setMat3(const char* varName, const glm::mat3& value) {
		this->checkUniform(varName);
		glUniformMatrix3fv(this->shaderUniforms[varName], 1, GL_FALSE, glm::value_ptr(value));
	}

	inline void setMat4(const char* varName, const glm::mat4& value) {
		this->checkUniform(varName);
		glUniformMatrix4fv(this->shaderUniforms[varName], 1, GL_FALSE, glm::value_ptr(value));
	}

	template <class T>
	void setArray(std::string varName, int count, std::vector<T>& values) {
		std::string buffer{};
		for (int i = 0; i < count; i++) {
			buffer = varName + "[" + std::to_string(i) + "]";

			this->checkUniform(buffer.c_str());

			if (typeid(values[0]).name() == "int")
				glUniform1i(this->shaderUniforms[buffer.c_str()], values[i]);
			else if (typeid(values[0]).name() == "unsigned int")
				glUniform1ui(this->shaderUniforms[buffer.c_str()], values[i]);
			else if (typeid(values[0]).name() == "float")
				glUniform1f(this->shaderUniforms[buffer.c_str()], values[i]);
		}
	}

	~Shader();
};