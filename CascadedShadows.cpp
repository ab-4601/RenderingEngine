#include "CascadedShadows.h"

CascadedShadows::CascadedShadows(int windowWidth, int windowHeight, float lambda, int size, int samplesU, int samplesV)
	: aspect{ (float)windowWidth / windowHeight } 
{
	for (int i = 0; i < ::MAX_CASCADES; i++)
		cascadeSplits[i] = 0;

	genRandomOffsetData(size, samplesU, samplesV);
	calcSplitDepths(lambda);
	setComputeUniforms();
	_init();
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

	noiseTextureSize = glm::vec3(size, size, samples / 2);

	glGenTextures(1, &randomOffset);
	glBindTexture(GL_TEXTURE_3D, randomOffset);

	glTexStorage3D(GL_TEXTURE_3D, 1, GL_RGBA32F, size, size, samples / 2);
	glTexSubImage3D(GL_TEXTURE_3D, 0, 0, 0, 0, size, size, samples / 2, GL_RGBA, GL_FLOAT, offsetData);

	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	delete[] offsetData;
}

void CascadedShadows::_init() {
	glGenFramebuffers(1, &FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);

	glGenTextures(1, &shadowMaps);
	glBindTexture(GL_TEXTURE_2D_ARRAY, shadowMaps);

	glTexImage3D(
		GL_TEXTURE_2D_ARRAY, 0, GL_DEPTH_COMPONENT, mapResolution, mapResolution,
		numCascades + 1, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL
	);

	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	constexpr float borderColor[] = { 1.f, 1.f, 1.f, 1.f };
	glTexParameterfv(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_BORDER_COLOR, borderColor);

	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, shadowMaps, 0);

	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	if (!checkFramebufferStatus()) {
		std::cerr << "Error initializing CSM framebuffer" << std::endl;
		exit(-1);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glGenBuffers(1, &SSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, SSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(glm::mat4x4) * 16, NULL, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, SSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void CascadedShadows::calcSplitDepths(float lambda) {
	float range = ::far_plane - ::near_plane;
	float logFactor = std::log(::far_plane / ::near_plane);

	for (int i = 0; i < numCascades; i++) {
		float p = (float)(i + 1) / numCascades;

		float linearSplit = ::near_plane + range * p;

		float logSplit = ::near_plane * std::pow(logFactor, p);

		cascadeSplits[i] = (lambda * linearSplit + (1 - lambda) * logSplit);
	}
}

void CascadedShadows::setComputeUniforms() {
	computeShader.useShader();

	char buffer[50]{ '\0' };

	for (int i = 0; i < ::MAX_CASCADES; i++) {
		snprintf(buffer, sizeof(buffer), "cascadeSplits[%i]", i);
		this->computeShader.setFloat(buffer, cascadeSplits[i]);
	}

	computeShader.setFloat("nearPlane", ::near_plane);
	computeShader.setFloat("farPlane", ::far_plane);
	computeShader.setFloat("aspect", aspect);
	computeShader.setInt("numCascades", numCascades);

	computeShader.endShader();
}

void CascadedShadows::calculateShadows(int windowWidth, int windowHeight, const std::vector<Mesh*>& meshes,
	const std::vector<Model*>& models, glm::vec3 lightPosition, GLuint currFramebuffer)
{
	computeShader.useShader();
	computeShader.setVec3("lightDirection", lightPosition);
	
	computeShader.dispatchComputeShader(1, 1, 1, GL_SHADER_STORAGE_BARRIER_BIT);

	computeShader.endShader();

	shader.useShader();

	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	glClear(GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, mapResolution, mapResolution);

	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(3.f, 3.f);

	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_CLAMP);
	glCullFace(GL_FRONT);

	for (size_t i = 0; i < meshes.size(); i++) {
		shader.setMat4("model", meshes[i]->getModelMatrix());
		shader.setUint("useDiffuseMap", meshes[i]->getDiffuseMapBool());
		meshes[i]->drawMesh(GL_TRIANGLES);
	}

	for (size_t i = 0; i < models.size(); i++) {
		shader.setMat4("model", models[i]->getModelMatrix());
		shader.setUint("useDiffuseMap", models[i]->getDiffuseMapBool());
		models[i]->drawModel(GL_TRIANGLES);
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
	if (FBO != 0)
		glDeleteFramebuffers(1, &FBO);

	if (SSBO != 0)
		glDeleteBuffers(1, &SSBO);

	if (shadowMaps != 0)
		glDeleteTextures(1, &shadowMaps);
}