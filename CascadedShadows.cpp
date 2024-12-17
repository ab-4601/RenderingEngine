#include "CascadedShadows.h"

CascadedShadows::CascadedShadows(int windowWidth, int windowHeight, float lambda, int size, int samplesU, int samplesV)
	: aspect{ (float)windowWidth / windowHeight } 
{
	for (int i = 0; i < ::MAX_CASCADES; i++) {
		this->cascadeSplits[i] = 0;
		this->lightSpaceMatrices[i] = glm::mat4(1.f);
	}

	this->calcSplitDepths(lambda);
	this->setComputeUniforms();
	this->genRandomOffsetData(size, samplesU, samplesV);
	this->_init();
}

bool CascadedShadows::checkFramebufferStatus() {
	uint status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	if (status != GL_FRAMEBUFFER_COMPLETE)
		return false;

	return true;
}

void CascadedShadows::genRandomOffsetData(int size, int samplesU, int samplesV) {
	int samples = samplesU * samplesV;
	int bufferSize = size * size * samples * 2;
	float* offsetData = new float[bufferSize];

	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			for (int k = 0; k < samples; k += 2) {
				int x1{}, y1{}, x2{}, y2{};
				x1 = k % (samplesU);
				y1 = (samples - 1 - k) / samplesU;
				x2 = (k + 1) % samplesU;
				y2 = (samples - 1 - k - 1) / samplesU;

				glm::vec4 v{};

				v.x = (x1 + 0.5f) + jitter(-0.5f, 0.5f);
				v.y = (y1 + 0.5f) + jitter(-0.5f, 0.5f);
				v.z = (x2 + 0.5f) + jitter(-0.5f, 0.5f);
				v.w = (y2 + 0.5f) + jitter(-0.5f, 0.5f);

				v.x /= samplesU;
				v.y /= samplesV;
				v.z /= samplesU;
				v.w /= samplesV;

				int cell = ((k / 2) * size * size + j * size + i) * 4;

				offsetData[cell + 0] = glm::sqrt(v.y) * glm::cos(glm::two_pi<float>() * v.x);
				offsetData[cell + 1] = glm::sqrt(v.y) * glm::sin(glm::two_pi<float>() * v.x);
				offsetData[cell + 2] = glm::sqrt(v.w) * glm::cos(glm::two_pi<float>() * v.z);
				offsetData[cell + 3] = glm::sqrt(v.w) * glm::sin(glm::two_pi<float>() * v.z);
			}
		}
	}

	this->noiseTextureSize = glm::vec3(size, size, samples / 2);

	glGenTextures(1, &this->randomOffset);
	glBindTexture(GL_TEXTURE_3D, this->randomOffset);

	glTexStorage3D(GL_TEXTURE_3D, 1, GL_RGBA32F, size, size, samples / 2);
	glTexSubImage3D(GL_TEXTURE_3D, 0, 0, 0, 0, size, size, samples / 2, GL_RGBA, GL_FLOAT, offsetData);

	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	delete[] offsetData;
}

void CascadedShadows::_init() {
	glGenFramebuffers(1, &this->FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, this->FBO);

	glGenTextures(1, &this->shadowMaps);
	glBindTexture(GL_TEXTURE_2D_ARRAY, this->shadowMaps);

	glTexStorage3D(
		GL_TEXTURE_2D_ARRAY, 1, GL_DEPTH_COMPONENT32F, this->mapResolution, this->mapResolution, this->numCascades + 1
	);

	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_SPARSE_ARB, GL_TRUE);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	constexpr float borderColor[] = { 1.f, 1.f, 1.f, 1.f };
	glTexParameterfv(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_BORDER_COLOR, borderColor);

	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, this->shadowMaps, 0);

	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	if (!this->checkFramebufferStatus()) {
		std::cerr << "Error initializing CSM framebuffer" << std::endl;
		exit(-1);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glGenBuffers(1, &this->SSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, this->SSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(glm::mat4x4) * 16, nullptr, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, this->SSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void CascadedShadows::calcSplitDepths(float lambda) {
	float range = ::far_plane - ::near_plane;
	float logFactor = std::log(::far_plane / ::near_plane);

	for (int i = 0; i < this->numCascades; i++) {
		float p = (float)(i + 1) / this->numCascades;

		float linearSplit = ::near_plane + range * p;

		float logSplit = ::near_plane * std::pow(logFactor, p);

		this->cascadeSplits[i] = (lambda * linearSplit + (1 - lambda) * logSplit);
	}
}

void CascadedShadows::setComputeUniforms() {
	this->computeShader.useShader();

	std::string buffer{};
	for (int i = 0; i < this->numCascades; i++) {
		buffer = "cascadeSplits[" + std::to_string(i) + "]";
		this->computeShader.setFloat(buffer.c_str(), this->cascadeSplits[i]);
	}

	this->computeShader.setFloat("nearPlane", ::near_plane);
	this->computeShader.setFloat("farPlane", ::far_plane);
	this->computeShader.setFloat("aspect", this->aspect);
	this->computeShader.setInt("numCascades", this->numCascades);

	this->computeShader.endShader();
}

void CascadedShadows::calculateShadows(int windowWidth, int windowHeight, std::vector<Mesh*>& meshes,
	glm::vec3 lightPosition, GLuint currFramebuffer)
{
	this->computeShader.useShader();
	this->computeShader.setVec3("lightDirection", lightPosition);
	
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, this->SSBO);
	this->computeShader.dispatchComputeShader(1, 1, 1, GL_SHADER_STORAGE_BARRIER_BIT);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	this->computeShader.endShader();

	this->shader.useShader();

	glBindFramebuffer(GL_FRAMEBUFFER, this->FBO);
	glClear(GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, this->mapResolution, this->mapResolution);

	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(3.f, 3.f);

	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_CLAMP);
	glCullFace(GL_FRONT);

	for (size_t i = 0; i < meshes.size(); i++) {
		this->shader.setMat4("model", meshes[i]->getModelMatrix());
		meshes[i]->drawMesh(GL_TRIANGLES);
	}

	glCullFace(GL_BACK);
	glDisable(GL_CULL_FACE);

	glDisable(GL_POLYGON_OFFSET_FILL);
	glDisable(GL_DEPTH_CLAMP);

	this->shader.endShader();

	glViewport(0, 0, windowWidth, windowHeight);
	glBindFramebuffer(GL_FRAMEBUFFER, currFramebuffer);
}

CascadedShadows::~CascadedShadows() {
	if (this->FBO != 0)
		glDeleteFramebuffers(1, &this->FBO);

	if (this->shadowMaps != 0)
		glDeleteTextures(1, &this->shadowMaps);
}