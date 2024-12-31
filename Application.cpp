#include "Application.h"

Application::Application() {
	meshes.clear();
	models.clear();

    float windowWidth = (float)window.getBufferWidth();
    float windowHeight = (float)window.getBufferHeight();

    viewportMatrix = glm::mat4{
        glm::vec4(windowWidth / 2.f, 0.f, 0.f, windowWidth / 2.f),
        glm::vec4(0.f, windowHeight / 2.f, 0.f, windowHeight / 2.f),
        glm::vec4(0.f, 0.f, 1.f, 0.f),
        glm::vec4(0.f, 0.f, 0.f, 1.f)
    };

    quad.createQuad();
}

void Application::setLightingUniforms(Shader& shader) {
    shader.setVec3("directionalLight.base.color", skylight.getColor());
    shader.setVec3("directionalLight.direction", skylight.getLightDirection());

    if (pointLightCount > ::MAX_POINT_LIGHTS)
        pointLightCount = ::MAX_POINT_LIGHTS;

    if (spotLightCount > ::MAX_SPOT_LIGHTS)
        spotLightCount = ::MAX_SPOT_LIGHTS;

    shader.setUint("pointLightCount", pointLightCount);
    shader.setUint("spotLightCount", spotLightCount);

    char buffer[64]{ '\0' };

    for (uint i = 0; i < pointLightCount; i++) {

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

    for (uint i = 0; i < spotLightCount; i++) {
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

void Application::setGlobalPBRUniforms(Shader& shader) {
    shader.useShader();

    shader.setFloat("nearPlane", ::near_plane);
    shader.setFloat("farPlane", ::far_plane);
    shader.setInt("cascadeCount", csm.getNumCascades());
    shader.setVec3("offsetTexSize", csm.getNoiseTextureSize());
    shader.setMat4("viewportMatrix", viewportMatrix);

    const float* cascadePlanes = csm.cascadePlanes();

    char buffer[64]{ '\0' };

    for (int i = 0; i < ::MAX_CASCADES; i++) {
        snprintf(buffer, sizeof(buffer), "cascadePlanes[%i]", i);
        shader.setFloat(buffer, cascadePlanes[i]);
    }

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skybox.getIrradianceMap());

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skybox.getPrefilterTexture());

    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, skybox.getBRDFTexture());

    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    glActiveTexture(GL_TEXTURE5);
    glBindTexture(GL_TEXTURE_2D_ARRAY, csm.getShadowMaps());

    glActiveTexture(GL_TEXTURE6);
    glBindTexture(GL_TEXTURE_3D, csm.noiseBuffer());

    shader.endShader();
}

void Application::start() {
    glm::vec3 lightDirection(3000.f, 3000.f, 0.f);
    glm::vec3 pointLightPosition1(20.0, 20.f, 20.f);
    glm::vec3 pointLightPosition2(100.f, 30.f, 100.f);
    glm::vec3 spotLightPosition(300.0, 80.f, 300.f);

    glm::mat4 model{ 1.f };

    Icosphere* sphere = new Icosphere();
    sphere->smoothSphere(5);
    sphere->setColor({ 0.07f, 1.f, 1.f });
    sphere->setMeshMaterial(0.f, 0.f, 1.f);
    sphere->loadMesh();

    model = glm::mat4(1.f);
    model = glm::translate(model, glm::vec3(400.f, 100.f, 0.f));
    model = glm::scale(model, glm::vec3(100.f));

    sphere->setModelMatrix(model);

    meshes.push_back(sphere);

    model = glm::mat4(1.f);
    model = glm::translate(model, glm::vec3(0.f, 100.f, 0.f));
    model = glm::scale(model, glm::vec3(100.f));

    Cube* cube = new Cube();
    cube->setColor({ 1.f, 0.07f, 0.07f });
    cube->setModelMatrix(model);
    cube->setMeshMaterial(0.f, 0.f, 1.f);;
    cube->loadMesh(false, false);

    meshes.push_back(cube);

    model = glm::mat4(1.f);
    model = glm::scale(model, glm::vec3(5.f, 5.f, 5.f));

    /*Terrain terrain(500, 500);
    terrain.generateHeightMaps(3);
    terrain.setMeshMaterial(0.f, 1.f, 1.f);
    terrain.generateTerrain();
    terrain.loadMesh();
    terrain.setModelMatrix(model);
    terrain.setColor(glm::vec3(0.2f, 0.2f, 0.2f));*/

    /*Model* suntemple = new Model(
        "Models/SunTemple/SunTemple.fbx",
        "Models/SunTemple/Textures/",
        aiTextureType_DIFFUSE,
        aiTextureType_NORMALS,
        aiTextureType_SPECULAR,
        aiTextureType_EMISSIVE,
        true, true
    );*/

    model = glm::mat4(1.f);
    model = glm::scale(model, glm::vec3(100.f));

    /*Model* isometricRoom = new Model(
        "Models/IsometricRoom/Isometric Room v5058 p4866.fbx",
        "Models/IsometricRoom/",
        aiTextureType_DIFFUSE,
        aiTextureType_NORMALS,
        aiTextureType_METALNESS,
        aiTextureType_EMISSIVE,
        false, true
    );*/

    //isometricRoom->setModelMatrix(model);

    Model* sponza = new Model(
        "Models/Sponza/Sponza.gltf",
        "Models/Sponza/"
    );

    //models.push_back(isometricRoom);
    models.push_back(sponza);
    //models.push_back(suntemple);

    coordSystem.createCoordinateSystem();

    ParticleTexture partTex("Textures/particleAtlas.png", 4.f);
    glm::vec3 particlePosition{ 20.f, 20.f, 20.f }, velocity{ 10.f, 50.f, 10.f }, particleColor{ 1.f, 0.5f, 0.05f };
    ParticleSystem pSystem(particleColor, 10, 15.f, 1.f, 6.f, partTex);

    /*ParticleTexture fire("Textures/fire.png", 8.f);
    glm::vec3 fireParticlePosition{ 1125.f, 120.f, 400.f };
    ParticleSystem fireSystem(particleColor, 30.f, -30.f, 1.f, 30.f, fire);*/

    hdrBuffer._initMSAA();

    setGlobalPBRUniforms(forwardShader);
    //setGlobalPBRUniforms(deferredShader);

    while (!glfwWindowShouldClose(window.getGlfwWindow())) {
        mainLoopForward(pSystem, model, particlePosition, lightDirection);
    }
}

void Application::mainLoopForward(ParticleSystem& pSystem, glm::mat4& model, glm::vec3& particlePosition, glm::vec3& lightDirection) {
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

    pSystem.updateParticles(deltaTime, camera.getCameraPosition());
    //fireSystem.updateParticles(deltaTime, this->camera.getCameraPosition());

    if (elapsedTime >= 0.012f)
    {
        currFramebuffer = 0;
        elapsedTime = 0.f;

        gui._newFrame();

        currFramebuffer = 0;

        skylight.updateLightLocation(lightDirection);

        if (enableHDR) {
            hdrBuffer.enableHDRWriting();
            currFramebuffer = hdrBuffer.getFramebufferID();
        }

        // Clear window
        glClearColor(0.f, 0.f, 0.f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        if (drawSkybox)
            skybox.renderSkybox();

// ----------------------------------------------------------------------------------------------------------------

        lightSources.renderLightSources(
            skylight, pointLights, spotLights, pointLightCount, spotLightCount
        );

// ----------------------------------------------------------------------------------------------------------------

        if (enableShadows) {
            csm.calculateShadows(
                window.getBufferWidth(), window.getBufferHeight(), meshes,
                models, lightDirection, currFramebuffer
            );
        }

        selection.pickingPhase(meshes, currFramebuffer);

        mouseClickCoords = window.getViewportCoord();

        if (window.getKeyPress(GLFW_KEY_TAB)) {
            index = -1;
            prevIndex = -1;
        }

        if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
            index = selection.mouseSelectionResult(
                window.getWindowHeight(), (int)mouseClickCoords.x, (int)mouseClickCoords.y
            );

            if (prevIndex != index)
                if (index != -1)
                    prevIndex = index;
                else
                    index = prevIndex;
        }

        forwardShader.useShader();

        setLightingUniforms(forwardShader);

        forwardShader.setUint("calcShadows", enableShadows);
        forwardShader.setUint("enableSSAO", enableSSAO);
        forwardShader.setUint("drawWireframe", drawWireframe);
        forwardShader.setFloat("radius", shadowRadius);

        for (size_t i = 0; i < models.size(); i++) {
            models[i]->renderModel(forwardShader, GL_TRIANGLES);
        }

        if (index < (int)meshes.size() && index != -1) {
            gui._updateTransformOperation(window);
            gui.manipulate(window.getWindowWidth(), window.getWindowHeight(), camera, meshes[index]);
            meshes[index]->renderMeshWithOutline(forwardShader, outlineShader, GL_TRIANGLES);
        }

        for (size_t i = 0; i < meshes.size(); i++) {
            if ((int)i != index && meshes[i]->getObjectID() != -1)
                meshes[i]->renderMesh(forwardShader, GL_TRIANGLES);
        }

// ----------------------------------------------------------------------------------------------------------------

        if (displayGrid)
            grid.renderGrid();

// ----------------------------------------------------------------------------------------------------------------

        if (displayCoordinateSystem)
            coordSystem.drawCoordinateSystem(window.getWindowHeight(), window.getWindowWidth(),
                window.getBufferWidth(), window.getBufferHeight(), camera);

// ----------------------------------------------------------------------------------------------------------------

        particlePosition = camera.getCameraPosition() + camera.getCameraLookDirection() * 200.f;
        //fireSystem.generateParticles(fireParticlePosition, 0.f);

        if (window.getKeyPress(GLFW_KEY_R)) {
            pSystem.generateParticles(particlePosition, 0.f);
        }

        pSystem.renderParticles(&window, camera, model);
        //fireSystem.renderParticles(&this->window, this->camera, model);

        if (enableHDR && enableBloom) {
            bloom.setKnee(bloomThreshold);
            bloom.renderBloomTextureMSAA(filterRadius, currFramebuffer);
        }

// ----------------------------------------------------------------------------------------------------------------

        if (enableHDR) {
            //hdrBuffer.renderToDefaultBuffer(exposure, bloom.bloomTexture(), enableBloom);
            hdrBuffer.renderToDefaultBufferMSAA(exposure, bloom.bloomTexture(), enableBloom);
        }

        if (index != -1)
            gui.render(io, exposure, shadowRadius, filterRadius, bloomThreshold, ssaoRadius,
                ssaoBias, ssaoOcclusionPower, drawSkybox, displayGrid, displayCoordinateSystem,
                enableBloom, drawWireframe, enableShadows, enableHDR, enableSSAO, lightDirection, meshes[index]);
        else
            gui.render(io, exposure, shadowRadius, filterRadius, bloomThreshold, ssaoRadius,
                ssaoBias, ssaoOcclusionPower, drawSkybox, displayGrid, displayCoordinateSystem,
                enableBloom, drawWireframe, enableShadows, enableHDR, enableSSAO, lightDirection);

        glfwSwapBuffers(window.getMainWindow());
    }
}

void Application::mainLoopDeferred(ParticleSystem& pSystem, glm::mat4& model, glm::vec3& particlePosition, glm::vec3& lightDirection) {
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

    pSystem.updateParticles(deltaTime, camera.getCameraPosition());
    //fireSystem.updateParticles(deltaTime, this->camera.getCameraPosition());

    if (elapsedTime >= 0.012f)
    {
        currFramebuffer = 0;
        elapsedTime = 0.f;

        gui._newFrame();

        currFramebuffer = 0;

        skylight.updateLightLocation(lightDirection);

        if (enableHDR) {
            hdrBuffer.enableHDRWriting();
            currFramebuffer = hdrBuffer.getFramebufferID();
        }

        // Clear window
        glClearColor(0.f, 0.f, 0.f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        if (drawSkybox)
            skybox.renderSkybox();

// ----------------------------------------------------------------------------------------------------------------

        lightSources.renderLightSources(
            skylight, pointLights, spotLights, pointLightCount, spotLightCount
        );

// ----------------------------------------------------------------------------------------------------------------

        if (enableShadows) {
            csm.calculateShadows(
                window.getBufferWidth(), window.getBufferHeight(), meshes,
                models, lightDirection, currFramebuffer
            );
        }

        selection.pickingPhase(meshes, currFramebuffer);

        mouseClickCoords = window.getViewportCoord();

        if (window.getKeyPress(GLFW_KEY_TAB)) {
            index = -1;
            prevIndex = -1;
        }

        if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
            index = selection.mouseSelectionResult(
                window.getWindowHeight(), (int)mouseClickCoords.x, (int)mouseClickCoords.y
            );

            if (prevIndex != index)
                if (index != -1)
                    prevIndex = index;
                else
                    index = prevIndex;
        }

        ssao.setRadius(ssaoRadius);
        ssao.setBias(ssaoBias);
        ssao.setOcclusionPower(ssaoOcclusionPower);
        gbuffer.updateWireframeBool(drawWireframe);
        gbuffer.updateBuffer(outlineShader, index, meshes, models, currFramebuffer);
        ssao.calcSSAO(gbuffer.positionBuffer(), gbuffer.normalBuffer(), currFramebuffer);

        deferredShader.useShader();

        setLightingUniforms(deferredShader);

        deferredShader.setUint("calcShadows", enableShadows);
        deferredShader.setUint("enableSSAO", enableSSAO);
        deferredShader.setFloat("radius", shadowRadius);

        glActiveTexture(GL_TEXTURE7);
        glBindTexture(GL_TEXTURE_2D, gbuffer.positionBuffer());

        glActiveTexture(GL_TEXTURE8);
        glBindTexture(GL_TEXTURE_2D, gbuffer.albedoBuffer());

        glActiveTexture(GL_TEXTURE9);
        glBindTexture(GL_TEXTURE_2D, gbuffer.normalBuffer());

        glActiveTexture(GL_TEXTURE10);
        glBindTexture(GL_TEXTURE_2D, gbuffer.metallicBuffer());

        glActiveTexture(GL_TEXTURE11);
        glBindTexture(GL_TEXTURE_2D, ssao.occlusionBuffer());

        if (index < (int)meshes.size() && index != -1) {
            gui._updateTransformOperation(window);
            gui.manipulate(window.getWindowWidth(), window.getWindowHeight(), camera, meshes[index]);
        }

        quad.renderQuad();

// ----------------------------------------------------------------------------------------------------------------

        if (displayGrid)
            grid.renderGrid();

// ----------------------------------------------------------------------------------------------------------------

        if (displayCoordinateSystem)
            coordSystem.drawCoordinateSystem(window.getWindowHeight(), window.getWindowWidth(),
                window.getBufferWidth(), window.getBufferHeight(), camera);

// ----------------------------------------------------------------------------------------------------------------

        particlePosition = camera.getCameraPosition() + camera.getCameraLookDirection() * 200.f;
        //fireSystem.generateParticles(fireParticlePosition, 0.f);

        if (window.getKeyPress(GLFW_KEY_R)) {
            pSystem.generateParticles(particlePosition, 0.f);
        }

        pSystem.renderParticles(&window, camera, model);
        //fireSystem.renderParticles(&window, camera, model);

        if (enableHDR && enableBloom) {
            bloom.setKnee(bloomThreshold);
            bloom.renderBloomTextureMSAA(filterRadius, currFramebuffer);
        }

// ----------------------------------------------------------------------------------------------------------------

        if (enableHDR) {
            //hdrBuffer.renderToDefaultBuffer(exposure, bloom.bloomTexture(), enableBloom);
            hdrBuffer.renderToDefaultBufferMSAA(exposure, bloom.bloomTexture(), enableBloom);
        }

        if (index != -1)
            gui.render(io, exposure, shadowRadius, filterRadius, bloomThreshold, ssaoRadius,
                ssaoBias, ssaoOcclusionPower, drawSkybox, displayGrid, displayCoordinateSystem,
                enableBloom, drawWireframe, enableShadows, enableHDR, enableSSAO, lightDirection, meshes[index]);
        else
            gui.render(io, exposure, shadowRadius, filterRadius, bloomThreshold, ssaoRadius,
                ssaoBias, ssaoOcclusionPower, drawSkybox, displayGrid, displayCoordinateSystem,
                enableBloom, drawWireframe, enableShadows, enableHDR, enableSSAO, lightDirection);

        glfwSwapBuffers(window.getMainWindow());
    }
}