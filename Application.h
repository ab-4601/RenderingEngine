#pragma once

#include "Skybox.h"
#include "Overlay.h"
#include "HDR.h"
#include "BloomRenderer.h"
#include "MouseSelector.h"
#include "CoordinateSystem.h"
#include "Cube.h"
#include "UVSphere.h"
#include "Icosphere.h"
#include "Terrain.h"
#include "GBuffer.h"
#include "SSAO.h"
#include "LightSources.h"
#include "CascadedShadows.h"
#include "DirectionalShadow.h"
#include "ShadowMap.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "ParticleSystem.h"
#include "Grid.h"

class Application {
private:
    bool drawSkybox = true;
    bool enableBloom = true;
    bool drawWireframe = false;
    bool enableShadows = false;
    bool enableSSAO = false;
    bool enableHDR = true;
    bool displayCoordinateSystem = false;
    bool displayGrid = false;

    float exposure = 1.f;
    float filterRadius = 0.005f;
    float shadowRadius = 2.f;
    float bloomThreshold = 1.f;
    float ssaoRadius = 50.f;
    float ssaoBias = 2.f;
    float ssaoOcclusionPower = 10.f;

    float deltaTime = 0.f;
    float lastTime = 0.f;
    float currTime = 0.f;
    float prevTime = 0.f;
    float elapsedTime = 0.f;

    glm::mat4 viewportMatrix{ 1.f };

    Quad quad;

    glm::vec2 mouseClickCoords{ 0.f };

    Window window{};
    Camera camera{ {-300, 500, 0}, window.getBufferWidth(), window.getBufferHeight() };
    Overlay gui{};
    HDR hdrBuffer{ window.getBufferWidth(), window.getBufferHeight() };
    BloomRenderer bloom{ window.getBufferWidth(), window.getBufferHeight() };
    Grid grid{};
    MouseSelector selection{ window.getWindowWidth(), window.getWindowHeight() };
    CoordinateSystem coordSystem;
    Skybox skybox{ window.getBufferWidth(), window.getBufferHeight() };
    DirectionalLight skylight{ 0.1f, 0.5f, { 4000.f, 4000.f, 0.f }, { 1.f, 1.f, 1.f } };
    LightSources lightSources{};
    CascadedShadows csm{ window.getBufferWidth(), window.getBufferHeight(), 0.4f };
    GBuffer gbuffer{ window.getBufferWidth(), window.getBufferHeight() };
    SSAO ssao{ window.getBufferWidth(), window.getBufferHeight() };

    std::vector<PointLight> pointLights{ ::MAX_POINT_LIGHTS, NULL };
    std::vector<SpotLight> spotLights{ ::MAX_SPOT_LIGHTS, NULL };

    ImGuiIO& io = gui._init(window.getGlfwWindow());

    uint pointLightCount = 0;
    uint spotLightCount = 0;

    GLuint currFramebuffer = 0;

    std::vector<Mesh*> meshes;
    std::vector<Model*> models;

    int index{ -1 }, prevIndex{ -1 };

    Shader forwardShader{ "PBR_forward.vert", "PBR_forward.frag", "PBR_forward.geom" };
    Shader deferredShader{ "PBR_deferred.vert", "PBR_deferred.frag" };
    Shader outlineShader{ "highlight.vert", "highlight.frag" };

    void setLightingUniforms(Shader& shader);
    void setGlobalPBRUniforms(Shader& shader);

    void mainLoopForward(ParticleSystem& pSystem, glm::mat4& model, glm::vec3& lightDirection);
    void mainLoopDeferred(ParticleSystem& pSystem, glm::mat4& model, glm::vec3& lightDirection);

public:
	Application();

	void start();

    ~Application() = default;
};

