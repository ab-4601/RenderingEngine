// main.cpp : This file contains the 'main' function. Program execution begins and ends there.

#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#endif

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
#include "LightSources.h"
#include "CascadedShadows.h"
#include "DirectionalShadow.h"
#include "ShadowMap.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "ParticleSystem.h"
#include "Grid.h"

uint Mesh::meshCount = 0;
std::vector<Mesh*> Mesh::meshList = {};

using namespace std;

const float toRadians = (float)PI / 180;
float rotationAngle = 1.f;

GLfloat deltaTime = 0.f;
GLfloat lastTime = 0.f;
GLfloat currTime = 0.f;
GLfloat prevTime = 0.f;
GLfloat elapsedTime = 0.f;

float filterRadius = 0.005f;

static void setLightingUniforms(Shader& shader, const DirectionalLight& light, const std::vector<PointLight>& pointLights,
    uint pointLightCount, const std::vector<SpotLight>& spotLights, uint spotLightCount) 
{
    shader.setVec3("directionalLight.base.color", light.getColor());
    shader.setVec3("directionalLight.direction", light.getLightDirection());

    if (pointLightCount > ::MAX_POINT_LIGHTS)
        pointLightCount = ::MAX_POINT_LIGHTS;

    if (spotLightCount > ::MAX_SPOT_LIGHTS)
        spotLightCount = ::MAX_SPOT_LIGHTS;

    shader.setUint("pointLightCount", pointLightCount);
    shader.setUint("spotLightCount", spotLightCount);

    char buffer[64]{ '\0' };

    for (int i = 0; i < pointLightCount; i++) {

        snprintf(buffer, sizeof(buffer), "pointLights[%i].base.color", i);
        shader.setVec3(buffer, pointLights[i].getColor());

        snprintf(buffer, sizeof(buffer), "pointLights[%i].position", i);
        shader.setVec3(buffer, pointLights[i].getPosition());

        snprintf(buffer, sizeof(buffer), "pointLights[%i].constant", i);
        shader.setFloat(buffer, pointLights[i].getAttenuationConstant());

        snprintf(buffer, sizeof(buffer), "pointLights[%i].linear", i);
        shader.setFloat(buffer, pointLights[i].getAttenuationLinear());

        snprintf(buffer, sizeof(buffer), "pointLights[%i].exponent", i);
        shader.setFloat(buffer, pointLights[i].getAttenuationExponent());
    }

    for (int i = 0; i < spotLightCount; i++) {
        snprintf(buffer, sizeof(buffer), "spotLights[%i].base.base.color", i);
        shader.setVec3(buffer, spotLights[i].getColor());

        snprintf(buffer, sizeof(buffer), "spotLights[%i].base.position", i);
        shader.setVec3(&buffer[0], spotLights[i].getPosition());

        snprintf(buffer, sizeof(buffer), "spotLights[%i].base.constant", i);
        shader.setFloat(&buffer[0], spotLights[i].getAttenuationConstant());

        snprintf(buffer, sizeof(buffer), "spotLights[%i].base.linear", i);
        shader.setFloat(&buffer[0], spotLights[i].getAttenuationLinear());

        snprintf(buffer, sizeof(buffer), "spotLights[%i].base.exponent", i);
        shader.setFloat(&buffer[0], spotLights[i].getAttenuationExponent());

        snprintf(buffer, sizeof(buffer), "spotLights[%i].direction", i);
        shader.setVec3(&buffer[0], spotLights[i].getDirection());

        snprintf(buffer, sizeof(buffer), "spotLights[%i].edge", i);
        shader.setFloat(&buffer[0], spotLights[i].getProcEdge());
    }
}

static void setGlobalPBRUniforms(Shader& shader, int numCascades, const float* cascadePlanes,
    glm::vec3 offsetTextureSize, glm::mat4 viewportMatrix, GLuint irradianceMap, GLuint brdfSampler,
    GLuint prefilterSampler, GLuint noiseSampler, GLuint cascadedShadowMaps, GLuint pointShadowMap) 
{
    shader.useShader();

    shader.setFloat("nearPlane", ::near_plane);
    shader.setFloat("farPlane", ::far_plane);
    shader.setInt("cascadeCount", numCascades);
    shader.setVec3("offsetTexSize", offsetTextureSize);
    shader.setMat4("viewportMatrix", viewportMatrix);
    
    char buffer[64]{ '\0' };

    for (int i = 0; i < ::MAX_CASCADES; i++) {
        snprintf(buffer, sizeof(buffer), "cascadePlanes[%i]", i);
        shader.setFloat(buffer, cascadePlanes[i]);
    }

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterSampler);

    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, brdfSampler);

    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_CUBE_MAP, pointShadowMap);

    glActiveTexture(GL_TEXTURE5);
    glBindTexture(GL_TEXTURE_2D_ARRAY, cascadedShadowMaps);

    glActiveTexture(GL_TEXTURE6);
    glBindTexture(GL_TEXTURE_3D, noiseSampler);

    shader.endShader();
}

int main() {
    srand((uint)time(0));

    glm::vec3 lightDirection(3000.f, 3000.f, 0.f);
    glm::vec3 pointLightPosition1(20.0, 20.f, 20.f);
    glm::vec3 pointLightPosition2(100.f, 30.f, 100.f);
    glm::vec3 spotLightPosition(300.0, 80.f, 300.f);

    Window window;
    Camera camera{ {-300, 500, 0}, window.getBufferWidth(), window.getBufferHeight() };
    Overlay overlay;
    HDR hdrBuffer{ window.getBufferWidth(), window.getBufferHeight() };
    BloomRenderer bloom{ window.getBufferWidth(), window.getBufferHeight() };
    Grid grid;
    MouseSelector selection{ window.getWindowWidth(), window.getWindowHeight() };
    CoordinateSystem coordSystem;
    Skybox skybox{ window.getBufferWidth(), window.getBufferHeight() };
    DirectionalLight mainLight{ 0.1f, 0.5f, lightDirection, { 1.f, 1.f, 1.f } };
    LightSources lightSources;
    CascadedShadows csm{ window.getBufferWidth(), window.getBufferHeight(), 0.4f, 20, 6, 6 };

    Shader lightingShader{ "PBR.vert", "PBR.frag", "PBR.geom" };
    Shader outlineShader{ "highlight.vert", "highlight.frag" };

    std::vector<Mesh*> meshes{};
    std::vector<Model*> models{};

    float windowWidth = (float)window.getBufferWidth();
    float windowHeight = (float)window.getBufferHeight();

    glm::mat4 viewportMatrix {
        glm::vec4(windowWidth / 2.f, 0.f, 0.f, windowWidth / 2.f),
        glm::vec4(0.f, windowHeight / 2.f, 0.f, windowHeight / 2.f),
        glm::vec4(0.f, 0.f, 1.f, 0.f),
        glm::vec4(0.f, 0.f, 0.f, 1.f)
    };

    glm::mat4 model(1.f);
    glm::mat4 view(1.f);
    glm::vec3 cameraPosition{ 0.f };
    glm::vec3 color(0.f, 0.f, 0.f);

    std::vector<PointLight> pointLights(MAX_POINT_LIGHTS, NULL);
    std::vector<SpotLight> spotLights(MAX_SPOT_LIGHTS, NULL);

    uint pointLightCount = 0;
    uint spotLightCount = 0;

    int index{ -1 }, prevIndex{ -1 };

    /*pointLights.at(0) = PointLight(0.01f, 0.4f, pointLightPosition1, 1.f, 0.001f, 0.001f, { 500.f, 500.f, 500.f });
    pointLightCount++;

    pointLights.at(1) = PointLight(0.01f, 0.4f, pointLightPosition2, 1.f, 0.0001f, 0.0001f, { 500.f, 500.f, 500.f });
    pointLightCount++;

    spotLights.at(0) = SpotLight(0.01f, 0.4f, spotLightPosition, 1.f, 0.0001f, 0.0001f, { 0.f, 0.f, 500.f }, { 0.f, -1.f, 0.f }, 45);
    spotLightCount++;*/

    bool drawSkybox = true;
    bool enableBloom = true;
    bool drawWireframe = false;
    bool enableShadows = false;
    bool enableSSAO = false;
    bool enableHDR = true;
    bool displayCoordinateSystem = false;
    bool displayGrid = false;

    float exposure = 1.f;
    float shadowRadius = 2.f;
    float bloomThreshold = 1.f;

    GLuint gridSize = 500;

    Icosphere sphere;
    sphere.smoothSphere(5);
    sphere.setColor({ 0.07f, 1.f, 1.f });
    sphere.setMeshMaterial(0.f, 0.f, 1.f);
    sphere.loadMesh();

    model = glm::mat4(1.f);
    model = glm::translate(model, glm::vec3(400.f, 100.f, 0.f));
    model = glm::scale(model, glm::vec3(100.f, 100.f, 100.f));

    sphere.setModelMatrix(model);

    model = glm::mat4(1.f);
    model = glm::translate(model, glm::vec3(0.f, 100.f, 0.f));
    model = glm::scale(model, glm::vec3(100.f, 100.f, 100.f));

    Cube cube;
    cube.setColor({ 1.f, 0.07f, 0.07f });
    cube.setModelMatrix(model);
    cube.setMeshMaterial(0.f, 0.f, 1.f);;
    cube.loadMesh(false, false);

    model = glm::mat4(1.f);
    model = glm::scale(model, glm::vec3(5.f, 5.f, 5.f));

    /*Terrain terrain(gridSize, gridSize);
    terrain.generateHeightMaps(3);
    terrain.setMeshMaterial(0.f, 1.f, 1.f);
    terrain.generateTerrain();
    terrain.loadMesh();
    terrain.setModelMatrix(model);
    terrain.setColor(glm::vec3(0.2f, 0.2f, 0.2f));*/

    meshes = Mesh::meshList;

    /*Model suntemple(
        "Models/SunTemple/SunTemple.fbx",
        "Models/SunTemple/Textures/",
        aiTextureType_DIFFUSE,
        aiTextureType_NORMALS,
        aiTextureType_SPECULAR,
        aiTextureType_EMISSIVE,
        true, true
    );*/

    Model sponza(
        "Models/Sponza/Sponza.gltf",
        "Models/Sponza/"
    );

    models.push_back(&sponza);
    //models.push_back(&suntemple);

    coordSystem.createCoordinateSystem();

    ParticleTexture partTex("Textures/particleAtlas.png", 4.f);
    glm::vec3 particlePosition{ 20.f, 20.f, 20.f }, velocity{ 10.f, 50.f, 10.f }, particleColor{ 1.f, 0.5f, 0.05f };
    ParticleSystem pSystem(particleColor, 10.f, 15.f, 1.f, 6.f, partTex);

    /*ParticleTexture fire("Textures/fire.png", 8.f);
    glm::vec3 fireParticlePosition{ 1125.f, 120.f, 400.f };
    ParticleSystem fireSystem(particleColor, 30.f, -30.f, 1.f, 30.f, fire);*/

    ImGuiIO& io = overlay._init(window.getGlfwWindow());

    hdrBuffer._initMSAA();

    GLuint currFramebuffer = 0;

    setGlobalPBRUniforms(
        lightingShader, csm.getNumCascades(), csm.cascadePlanes(), csm.getNoiseTextureSize(),
        viewportMatrix, skybox.getIrradianceMap(), skybox.getBRDFTexture(), skybox.getPrefilterTexture(),
        csm.noiseBuffer(), csm.getShadowMaps(), 0
    );

    // main render loop
    while (!glfwWindowShouldClose(window.getMainWindow())) {
        // Calculate delta time
        currTime = (GLfloat)glfwGetTime();
        deltaTime = currTime - lastTime;
        elapsedTime += deltaTime;
        lastTime = currTime;

        /*if (rotationAngle >= 360.f)
            rotationAngle = 0.f;
        else
            rotationAngle += 0.0001;*/

        glfwPollEvents();

        camera.keyFunctionality(window.getCurrWindow(), deltaTime);
        camera.mouseFunctionality(window.getXChange(), window.getYChange(), window.getScrollChange());

        cameraPosition = camera.getCameraPosition();
        view = camera.generateViewMatrix();

        pSystem.updateParticles(deltaTime, cameraPosition);
        //fireSystem.updateParticles(deltaTime, cameraPosition);

        if (elapsedTime >= 0.012f)
        {
            currFramebuffer = 0;
            elapsedTime = 0.f;

            overlay._newFrame();

            currFramebuffer = 0;

            mainLight.updateLightLocation(lightDirection);

            if(enableHDR) {
                hdrBuffer.enableHDRWriting();
                currFramebuffer = hdrBuffer.getFramebufferID();
            }

            // Clear window
            glClearColor(0.f, 0.f, 0.f, 1.f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

            if (drawSkybox)
                skybox.renderSkybox();

// ----------------------------------------------------------------------------------------------------------------

            lightSources.renderLightSources(mainLight, pointLights, spotLights, pointLightCount, spotLightCount);

// ----------------------------------------------------------------------------------------------------------------

            if (enableShadows) {
                csm.calculateShadows(
                    window.getBufferWidth(), window.getBufferHeight(), meshes, models, lightDirection, currFramebuffer
                );
            }

            selection.pickingPhase(meshes, currFramebuffer);

            lightingShader.useShader();

            setLightingUniforms(lightingShader, mainLight, pointLights, pointLightCount, spotLights, spotLightCount);

            lightingShader.setUint("calcShadows", enableShadows);
            lightingShader.setUint("enableSSAO", enableSSAO);
            lightingShader.setUint("drawWireframe", drawWireframe);
            lightingShader.setFloat("radius", shadowRadius);

            sponza.renderModel(lightingShader);
            //suntemple.renderModel(lightingShader);

            glm::vec2 mouseClickCoords = window.getViewportCoord();

            if (window.getKeyPress(GLFW_KEY_TAB)) {
                index = -1;
                prevIndex = -1;
            }

            if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
                index = selection.mouseSelectionResult(window.getWindowHeight(), mouseClickCoords.x, mouseClickCoords.y);

                if (prevIndex != index)
                    if (index != -1)
                        prevIndex = index;
                    else
                        index = prevIndex;
            }

            if (index < meshes.size() && index != -1) {
                overlay._updateTransformOperation(window);

                overlay.manipulate(window.getWindowWidth(), window.getWindowHeight(), camera, meshes[index]);

                meshes[index]->renderMeshWithOutline(lightingShader, outlineShader, GL_TRIANGLES);
            }

            for (size_t i = 0; i < meshes.size(); i++) {
                if ((int)i != index && meshes[i]->getObjectID() != -1)
                    meshes[i]->renderMesh(lightingShader, GL_TRIANGLES);
            }

// ----------------------------------------------------------------------------------------------------------------

            if(displayGrid)
                grid.renderGrid(cameraPosition);

// ----------------------------------------------------------------------------------------------------------------

            if(displayCoordinateSystem)
                coordSystem.drawCoordinateSystem(window.getWindowHeight(), window.getWindowWidth(),
                    window.getBufferWidth(), window.getBufferHeight(), camera);

// ----------------------------------------------------------------------------------------------------------------

            particlePosition = cameraPosition + camera.getCameraLookDirection() * 200.f;
            //fireSystem.generateParticles(fireParticlePosition, 0.f);

            if (window.getKeyPress(GLFW_KEY_R)) {
                pSystem.generateParticles(particlePosition, 0.f);
            }

            pSystem.renderParticles(&window, camera, model);
            //fireSystem.renderParticles(&window, camera, model);

            if(enableHDR && enableBloom) {
                bloom.setKnee(bloomThreshold);
                bloom.renderBloomTextureMSAA(filterRadius, currFramebuffer);
            }

// ----------------------------------------------------------------------------------------------------------------

            if (enableHDR) {
                //hdrBuffer.renderToDefaultBuffer(exposure, bloom.bloomTexture(), enableBloom);
                hdrBuffer.renderToDefaultBufferMSAA(exposure, bloom.bloomTexture(), enableBloom);
            }

            if (index != -1) 
                overlay.renderGUI(io, exposure, shadowRadius, filterRadius, bloomThreshold,
                    drawSkybox, displayGrid, displayCoordinateSystem,
                    enableBloom, drawWireframe, enableShadows, enableHDR, enableSSAO, lightDirection, meshes[index]);
            else
                overlay.renderGUI(io, exposure, shadowRadius, filterRadius, bloomThreshold,
                    drawSkybox, displayGrid, displayCoordinateSystem,
                    enableBloom, drawWireframe, enableShadows, enableHDR, enableSSAO, lightDirection);

            glfwSwapBuffers(window.getMainWindow());
        }
    }

    return 0;
}