#include "Shader.h"

Shader::Shader(std::string vertFileName, std::string fragFileName, std::string geomFileName)
	: vertexShaderSource{ nullptr }, fragmentShaderSource{ nullptr }, geometryShaderSource{ nullptr },
	vertexShaderID{ 0 }, fragmentShaderID{ 0 }, geometryShaderID{ 0 }, computeShaderID{ 0 }, programID { 0 }
{
	std::vector<std::string> files{};
	files.push_back(vertFileName);
	files.push_back(fragFileName);

	readFile(vertFileName, vertexShaderSource);
	readFile(fragFileName, fragmentShaderSource);

	compileShader(vertFileName, vertexShaderID, vertexShaderSource, GL_VERTEX_SHADER);
	compileShader(fragFileName, fragmentShaderID, fragmentShaderSource, GL_FRAGMENT_SHADER);

	if (geomFileName != "") {
		files.push_back(geomFileName);
		readFile(geomFileName, geometryShaderSource);
		compileShader(geomFileName, geometryShaderID, geometryShaderSource, GL_GEOMETRY_SHADER);
		attachShader(files, { vertexShaderID, fragmentShaderID, geometryShaderID });
		glDeleteShader(geometryShaderID);
	}
	else {
		attachShader(files, { vertexShaderID, fragmentShaderID });
	}

	glDeleteShader(vertexShaderID);
	glDeleteShader(fragmentShaderID);

	files.clear();

	delete[] vertexShaderSource;
	delete[] fragmentShaderSource;
	delete[] geometryShaderSource;
}

Shader::Shader(std::string compFileName)
	: vertexShaderSource{ nullptr }, fragmentShaderSource{ nullptr }, geometryShaderSource{ nullptr },
	vertexShaderID{ 0 }, fragmentShaderID{ 0 }, geometryShaderID{ 0 }, computeShaderID{ 0 }, programID{ 0 }
{
	std::vector<std::string> files{};
	files.push_back(compFileName);

	readFile(compFileName, computeShaderSource);
	compileShader(compFileName, computeShaderID, computeShaderSource, GL_COMPUTE_SHADER);
	attachShader(files, { computeShaderID });

	glDeleteShader(computeShaderID);

	delete[] computeShaderSource;
}

void Shader::readFile(std::string fileName, char*& shader) {
	std::ifstream file;

	file.open(fileName, std::ios::in | std::ios::binary);

	if (!file) {
		std::cerr << "Unable to open shader file at location: " << fileName << std::endl;
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

void Shader::compileShader(std::string fileName, GLuint& shaderID, const char* shader, GLenum shaderType) const {
	shaderID = glCreateShader(shaderType);
	glShaderSource(shaderID, 1, &shader, NULL);
	glCompileShader(shaderID);

	int success{};
	char infoLog[512];
	glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);

	// Display shader compile status if errors
	if (!success) {
		glGetShaderInfoLog(shaderID, 512, NULL, infoLog);
		std::cerr << "Error. Shader compilation failed: " + fileName + "\n" << infoLog << std::endl;
	}
}

void Shader::attachShader(std::vector<std::string> files, std::vector<GLuint> shaderIDs) {
	programID = glCreateProgram();

	for (GLuint& id : shaderIDs)
		glAttachShader(programID, id);

	glLinkProgram(programID);

	// Get link status
	int success{};
	char infoLog[512];

	std::string buffer{"( "};
	for (const auto& elem : files) {
		buffer += (elem + " ");
	}
	buffer += ")";

	glGetProgramiv(programID, GL_LINK_STATUS, &success);

	if (!success) {
		glGetProgramInfoLog(programID, 512, NULL, infoLog);
		std::cerr << "Error. Program linking failed: " + buffer + "\n" << infoLog << std::endl;
	}

	glValidateProgram(programID);

	glGetProgramiv(programID, GL_VALIDATE_STATUS, &success);

	if (!success) {
		glGetProgramInfoLog(programID, 512, NULL, infoLog);
		std::cerr << "Error. Program validation failed: " + buffer + "\n" << infoLog << std::endl;
	}
}

Shader::~Shader() {
	glDeleteProgram(programID);
}