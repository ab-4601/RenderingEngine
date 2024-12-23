#pragma once

#include "ShaderLoader.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"

class PBRShader : public IShaderLoader {
private:
	GLuint uniformModel{ 0 }, uniformViewportMatrix{ 0 }, uniformColor{ 0 }, uniformStrippedNormalBool{ 0 };
	GLuint uniformCameraPosition{ 0 }, uniformAlbedo{ 0 }, uniformMetallic{ 0 }, uniformRoughness{ 0 }, uniformAo{ 0 };
	GLuint uniformTextureBool{ 0 }, uniformSSAObool{ 0 }, uniformWireframeBool{ 0 };
	GLuint uniformEmissiveSampler{ 0 }, uniformUseNormalMap{ 0 }, uniformUseMaterialMap{ 0 };
	GLuint uniformCSMSampler{ 0 }, uniformCascadePlaneDistances[::MAX_CASCADES], uniformCascadeCount{ 0 };
	GLuint uniformIrradianceSampler{ 0 }, uniformBRDFSampler{ 0 }, uniformPrefilterSampler{ 0 }, uniformSSAOSampler{ 0 };
	GLuint uniformDiffuseSampler{ 0 }, uniformNormalSampler{ 0 }, uniformMetallicSampler{ 0 }, uniformRoughnessSampler{ 0 };
	GLuint uniformHeightScale{ 0 }, uniformDepthSampler{ 0 }, uniformLightSpaceTransform{ 0 }, uniformCalcShadows{ 0 };
	GLuint uniformPointShadowSampler{ 0 }, uniformNearPlane{ 0 }, uniformFarPlane{ 0 };
	GLuint uniformRandomOffsetSampler{ 0 }, uniformRadius{ 0 }, uniformOffsetTextureSize{ 0 };

	GLuint uniformPointLightCount = 0;

	struct {
		GLuint uniformAmbientColor = 0;
		GLuint uniformAmbientIntensity = 0;
		GLuint uniformDiffuseIntensity = 0;

		GLuint uniformDirection = 0;
	} uniformDirectionalLight;

	struct {
		GLuint uniformColor = 0;
		GLuint uniformAmbientIntensity = 0;
		GLuint uniformDiffuseIntensity = 0;

		GLuint uniformPosition = 0;
		GLuint uniformConstant = 0;
		GLuint uniformLinear = 0;
		GLuint uniformExponent = 0;
	} uniformPointLight[MAX_POINT_LIGHTS];

	GLuint uniformSpotLightCount = 0;

	struct {
		GLuint uniformColor = 0;
		GLuint uniformAmbientIntensity = 0;
		GLuint uniformDiffuseIntensity = 0;

		GLuint uniformPosition = 0;
		GLuint uniformConstant = 0;
		GLuint uniformLinear = 0;
		GLuint uniformExponent = 0;

		GLuint uniformDirection = 0;
		GLuint uniformEdge = 0;
	} uniformSpotLight[MAX_SPOT_LIGHTS];

public:
	PBRShader(
		std::string vertFileName = "PBR.vert",
		std::string fragFileName = "PBR.frag",
		std::string geomFileName = "PBR.geom"
	);

	virtual void getUniformLocations() override;

	void setDirectionalLight(DirectionalLight* directionalLight);
	void setPointLights(PointLight* pointLights, unsigned int lightCount);
	void setSpotLights(SpotLight* spotLights, unsigned int lightCount);

	void setGeneralUniforms(DirectionalLight& directionalLight, std::vector<PointLight>& pointLights, int pointLightCount,
		std::vector<SpotLight>& spotLights, int spotLightCount, int numCascades, const float* const cascadePlanes,
		float shadowRadius, glm::vec3 offsetTextureSize, glm::mat4 viewportMat, GLuint irradianceMap, GLuint brdfSampler,
		GLuint prefilterSampler, GLuint noiseSampler, GLuint cascadedShadowMap, GLuint pointShadowMap);

	inline GLuint getUniformModel() const { return this->uniformModel; }
	inline GLuint getUniformColor() const { return this->uniformColor; }
	inline GLuint getUniformViewportMatrix() const { return this->uniformViewportMatrix; }
	inline GLuint getAmbientIntensityLocation() const { return this->uniformDirectionalLight.uniformAmbientIntensity; }
	inline GLuint getAmbientColorLocation() const { return this->uniformDirectionalLight.uniformAmbientColor; }
	inline GLuint getDiffuseIntensityLocation() const { return this->uniformDirectionalLight.uniformDiffuseIntensity; }
	inline GLuint getDirectionLocation() const { return this->uniformDirectionalLight.uniformDirection; }
	inline GLuint getUniformCameraPosition() const { return this->uniformCameraPosition; }
	inline GLuint getUniformTextureBool() const { return this->uniformTextureBool; }
	inline GLuint getUniformNormalMapBool() const { return this->uniformUseNormalMap; }
	inline GLuint getUniformWireframeBool() const { return this->uniformWireframeBool; }
	inline GLuint getUniformUseMaterialMap() const { return this->uniformUseMaterialMap; }
	inline GLuint getUniformDiffuseSampler() const { return this->uniformDiffuseSampler; }
	inline GLuint getUniformNormalSampler() const { return this->uniformNormalSampler; }
	inline GLuint getUniformDepthSampler() const { return this->uniformDepthSampler; }
	inline GLuint getUniformMetallicSampler() const { return this->uniformMetallicSampler; }
	inline GLuint getUniformRoughnessSampler() const { return this->uniformRoughnessSampler; }
	inline GLuint getUniformEmissiveSampler() const { return this->uniformEmissiveSampler; }
	inline GLuint getUniformIrradianceSampler() const { return this->uniformIrradianceSampler; }
	inline GLuint getUniformBRDFSampler() const { return this->uniformBRDFSampler; }
	inline GLuint getUniformPrefilterSampler() const { return this->uniformPrefilterSampler; }
	inline GLuint getUniformSSAOSampler() const { return this->uniformSSAOSampler; }
	inline GLuint getUniformHeightScale() const { return this->uniformHeightScale; }
	inline GLuint getUniformPointShadowSampler() const { return this->uniformPointShadowSampler; }
	inline GLuint getUniformNearPlane() const { return this->uniformNearPlane; }
	inline GLuint getUniformFarPlane() const { return this->uniformFarPlane; }
	inline GLuint getUniformCascadePlaneDistance(int index) const { return this->uniformCascadePlaneDistances[index]; }
	inline GLuint getUniformCascadeCount() const { return this->uniformCascadeCount; }
	inline GLuint getUniformCSMSampler() const { return this->uniformCSMSampler; }
	inline GLuint getUniformLightSpaceTransform() const { return this->uniformLightSpaceTransform; }
	inline GLuint getUniformShadowBool() const { return this->uniformCalcShadows; }
	inline GLuint getUniformSSAObool() const { return this->uniformSSAObool; }
	inline GLuint getUniformOffsetTextureSize() const { return this->uniformOffsetTextureSize; }
	inline GLuint getUniformOffsetSampler() const { return this->uniformRandomOffsetSampler; }
	inline GLuint getUniformPCFRadius() const { return this->uniformRadius; }
	inline GLuint getUniformStrippedNormalBool() const { return this->uniformStrippedNormalBool; }

	inline GLuint getUniformAlbedo() const { return this->uniformAlbedo; }
	inline GLuint getUniformMetallic() const { return this->uniformMetallic; }
	inline GLuint getUniformRoughness() const { return this->uniformRoughness; }
	inline GLuint getUniformAo() const { return this->uniformAo; }

	~PBRShader() = default;
};

