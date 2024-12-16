#include "ShaderLoader.h"

IShaderLoader::IShaderLoader(std::string vertFileName, std::string fragFileName, std::string geomFileName) 
	: vertexShader{ nullptr }, fragmentShader{ nullptr }, geometryShader{ nullptr },
	vertFileName{ vertFileName }, fragFileName{ fragFileName }, geomFileName{ geomFileName },
	vertexShaderID{ 0 }, fragmentShaderID{ 0 }, geometryShaderID{ 0 }, programID{ 0 } {


}

void IShaderLoader::readFile(std::string fileName, char*& shader) {
	std::ifstream file;

	file.open(fileName, std::ios::in | std::ios::binary);

	if (!file) {
		std::cerr << "Unable to open file" << std::endl;
		file.close();
		exit(0);
	}

	std::string shaderSource{};
	char ch{};

	while (file.get(ch)) {
		shaderSource += ch;
	}

	shader = new char[shaderSource.length() + 1];

	for (size_t i = 0; i < shaderSource.length(); i++) {
		shader[i] = shaderSource.at(i);
	}

	shader[shaderSource.length()] = NULL;
}

void IShaderLoader::compileShader(GLuint& shaderID, const char* shader, GLenum shaderType) {
	shaderID = glCreateShader(shaderType);
	glShaderSource(shaderID, 1, &shader, NULL);
	glCompileShader(shaderID);

	int success{};
	char infoLog[512];
	glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);

	// Display shader compile status if errors
	if (!success) {
		glGetShaderInfoLog(shaderID, 512, NULL, infoLog);
		std::cerr << "Error. Shader compilation failed\n" << infoLog << std::endl;
	}
}

void IShaderLoader::attachShader(std::vector<GLuint> shaderIDs) {
	this->programID = glCreateProgram();

	for (GLuint& id : shaderIDs)
		glAttachShader(this->programID, id);

	glLinkProgram(this->programID);

	// Get link status
	int success{};
	char infoLog[512];

	glGetProgramiv(this->programID, GL_LINK_STATUS, &success);

	if (!success) {
		glGetProgramInfoLog(this->programID, 512, NULL, infoLog);
		std::cerr << "Error. Program linking failed\n" << infoLog << std::endl;
	}

	glValidateProgram(this->programID);

	glGetProgramiv(this->programID, GL_VALIDATE_STATUS, &success);

	if (!success) {
		glGetProgramInfoLog(this->programID, 512, NULL, infoLog);
		std::cerr << "Error. Program validation failed\n" << infoLog << std::endl;
	}

	this->getUniformLocations();
}

IShaderLoader::~IShaderLoader() {
	delete[] this->vertexShader;
	delete[] this->fragmentShader;
	delete[] this->geometryShader;

	glDeleteShader(this->vertexShaderID);
	glDeleteShader(this->fragmentShaderID);

	if(this->geometryShaderID != 0)
		glDeleteShader(this->geometryShaderID);

	glDeleteProgram(this->programID);
}