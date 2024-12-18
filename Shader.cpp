#include "Shader.h"

Shader::Shader(std::string vertFileName, std::string fragFileName, std::string geomFileName)
	: vertexShaderSource{ nullptr }, fragmentShaderSource{ nullptr }, geometryShaderSource{ nullptr },
	vertFileName{ vertFileName }, fragFileName{ fragFileName }, geomFileName{ geomFileName }, compFileName{ "" },
	vertexShaderID{ 0 }, fragmentShaderID{ 0 }, geometryShaderID{ 0 }, computeShaderID{ 0 }, programID { 0 }
{
	this->readFile(this->vertFileName, this->vertexShaderSource);
	this->readFile(this->fragFileName, this->fragmentShaderSource);

	this->compileShader(this->vertexShaderID, this->vertexShaderSource, GL_VERTEX_SHADER);
	this->compileShader(this->fragmentShaderID, this->fragmentShaderSource, GL_FRAGMENT_SHADER);

	if (this->geomFileName != "") {
		this->readFile(this->geomFileName, this->geometryShaderSource);
		this->compileShader(this->geometryShaderID, this->geometryShaderSource, GL_GEOMETRY_SHADER);
		this->attachShader({ vertexShaderID, fragmentShaderID, geometryShaderID });
		glDeleteShader(this->geometryShaderID);
	}
	else {
		this->attachShader({ vertexShaderID, fragmentShaderID });
	}

	glDeleteShader(this->vertexShaderID);
	glDeleteShader(this->fragmentShaderID);

	delete[] this->vertexShaderSource;
	delete[] this->fragmentShaderSource;
	delete[] this->geometryShaderSource;
}

Shader::Shader(std::string compFileName)
	: vertexShaderSource{ nullptr }, fragmentShaderSource{ nullptr }, geometryShaderSource{ nullptr },
	vertFileName{ "" }, fragFileName{ "" }, geomFileName{ "" }, compFileName{ compFileName },
	vertexShaderID{ 0 }, fragmentShaderID{ 0 }, geometryShaderID{ 0 }, computeShaderID{ 0 }, programID{ 0 }
{
	this->readFile(this->compFileName, this->computeShaderSource);
	this->compileShader(this->computeShaderID, this->computeShaderSource, GL_COMPUTE_SHADER);
	this->attachShader({ this->computeShaderID });

	glDeleteShader(this->computeShaderID);
}

void Shader::readFile(std::string fileName, char*& shader) {
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

void Shader::compileShader(GLuint& shaderID, const char* shader, GLenum shaderType) {
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

void Shader::attachShader(std::vector<GLuint> shaderIDs) {
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
}

Shader::~Shader() {
	glDeleteProgram(this->programID);
}