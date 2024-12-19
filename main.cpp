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
#include "Icosphere.h"
#include "Cube.h"
#include "UVSphere.h"
#include "Model.h"
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

int main() {
    srand((uint)time(0));

    glm::vec3 lightDirection(3000.f, 3000.f, 0.f);
    glm::vec3 pointLightPosition1(20.0, 20.f, 20.f);
    glm::vec3 pointLightPosition2(100.f, 30.f, 100.f);
    glm::vec3 spotLightPosition(300.0, 80.f, 300.f);

    Window window;
    Camera camera{ {-300, 100, 0}, window.getBufferWidth(), window.getBufferHeight() };
    Overlay overlay;
    HDR hdrBuffer{ window.getBufferWidth(), window.getBufferHeight() };
    BloomRenderer bloom{ (int)window.getWindowWidth(), (int)window.getWindowHeight() };
    Grid grid;
    MouseSelector selection{ (uint)window.getBufferWidth(), (uint)window.getBufferHeight() };
    CoordinateSystem coordSystem;
    Skybox skybox{ window.getBufferWidth(), window.getBufferHeight() };
    DirectionalLight mainLight{ 0.1f, 0.5f, lightDirection, { 1.f, 1.f, 1.f } };
    LightSources lightSources;
    CascadedShadows csm(window.getBufferWidth(), window.getBufferHeight());

    PBRShader pbrShader;
    Shader outlineShader{ "highlight.vert", "highlight.frag" };

    std::vector<Mesh*> meshes{};
    std::vector<Model*> models{};

    GLfloat aspect = (float)window.getBufferWidth() / window.getBufferHeight();

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

    pointLights.at(1) = PointLight(0.01f, 0.4f, pointLightPosition2, 1.f, 0.0001f, 0.0001f, {500.f, 500.f, 500.f});
    pointLightCount++;

    spotLights.at(0) = SpotLight(0.01f, 0.4f, spotLightPosition, 1.f, 0.0001f, 0.0001f,
        { 0.f, 0.f, 500.f }, { 0.f, -1.f, 0.f }, 45);
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
    float shadowRadius = 1.5f;
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
    terrain.loadMesh(true);
    terrain.setModelMatrix(model);
    terrain.setColor(glm::vec3(0.2f, 0.2f, 0.2f));*/

    meshes = Mesh::meshList;

    /*Model suntemple(
        "Models/SunTemple/SunTemple.fbx",
        "Models/SunTemple/Textures/",
        aiTextureType_DIFFUSE,
        aiTextureType_NORMALS,
        aiTextureType_SPECULAR,
        true
    );*/

    Model sponza(
        "Models/Sponza/Sponza.gltf",
        "Models/Sponza/",
        aiTextureType_DIFFUSE,
        aiTextureType_NORMALS,
        aiTextureType_METALNESS
    );

    //models.push_back(&sponza);
    //models.push_back(&suntemple);

    coordSystem.createCoordinateSystem();

    ParticleTexture partTex("Textures/particleAtlas.png", 4.f);
    glm::vec3 particlePosition{ 20.f, 20.f, 20.f }, velocity{ 10.f, 50.f, 10.f }, particleColor{ 1.f, 0.5f, 0.05f };
    ParticleSystem pSystem(particleColor, 10.f, 15.f, 1.f, 6.f, partTex);

    /*ParticleTexture fire("Textures/fire.png", 8.f);
    glm::vec3 fireParticlePosition{ 1125.f, 120.f, 400.f };
    ParticleSystem fireSystem(particleColor, 30, -30.f, 1.f, 30.f, fire);*/

    ImGuiIO& io = overlay._init(window.getGlfwWindow());

    hdrBuffer._initMSAA();

    GLuint currFramebuffer = 0;

    pbrShader.setGeneralUniforms(
        mainLight, pointLights, pointLightCount, spotLights, spotLightCount,
        csm.getNumCascades(), csm.cascadePlanes(), shadowRadius, csm.getNoiseTextureSize(), skybox.getIrradianceMap(),
        skybox.getBRDFTexture(), skybox.getPrefilterTexture(), csm.noiseBuffer(), csm.getShadowMaps(), 0
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
                    window.getWindowWidth(), window.getWindowHeight(), Mesh::meshList, lightDirection, currFramebuffer
                );
            }

            selection.pickingPhase(meshes, currFramebuffer);

            glUseProgram(pbrShader.getProgramID());

            pbrShader.setDirectionalLight(&mainLight);
            pbrShader.setPointLights(pointLights.data(), pointLightCount);
            pbrShader.setSpotLights(spotLights.data(), spotLightCount);

            glUniform1i(pbrShader.getUniformShadowBool(), enableShadows);
            glUniform1i(pbrShader.getUniformSSAObool(), enableSSAO);
            glUniform1f(pbrShader.getUniformPCFRadius(), shadowRadius);

            if (drawWireframe)
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

            sponza.renderModel(pbrShader, cameraPosition);
            //suntemple.renderModel(pbrShader, cameraPosition);

            glm::vec2 mouseClickCoords = window.getViewportCoord();

            if (window.getKeyPress(GLFW_KEY_TAB))
            {
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

                meshes[index]->renderMeshWithOutline(pbrShader, outlineShader, GL_TRIANGLES, cameraPosition);
            }

            for (size_t i = 0; i < meshes.size(); i++) {
                if ((int)i != index && meshes[i]->getObjectID() != -1)
                    meshes[i]->renderMesh(pbrShader, cameraPosition, GL_TRIANGLES);
            }

            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

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