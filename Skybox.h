#pragma once

#include "Camera.h"
#include "Texture.h"
#include "Quad.h"
#include "Shader.h"

class Skybox : public Texture {
private:
    static const int CUBEMAP_RESOLUTION = 512;
    static const int CONVOLUTION_RESOLUTION = 32;
    static const int PREFILTER_RESOLUTION = 512;

    GLuint VAO{ 0 }, VBO{ 0 };

    GLfloat vertices[108] = {
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };

    glm::mat4 projection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
    glm::mat4 viewMatrices[6] = {
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
    };

    Shader skyboxShader{ "skybox.vert", "skybox.frag" };
    Shader hdrToCubeShader{ "HDRskybox.vert", "HDRskybox.frag" };
    Shader irradianceShader{ "HDRskybox.vert", "irradiance.frag" };
    Shader prefilterShader{ "HDRskybox.vert", "prefilter.frag" };
    Shader brdfShader{ "brdf.vert", "brdf.frag" };

    GLuint FBO{ 0 }, RBO{ 0 }, environmentMap{ 0 };
    GLuint irradianceMap{ 0 }, prefilterMap{ 0 }, brdfTexture{ 0 };

    Quad quad;

    void renderCube() const {
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);

        glDrawArrays(GL_TRIANGLES, 0, 36);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    void _initFBO();
    void _generateCubemap();
    void _generateIrradianceMap();
    void _generatePrefilterMipmap();
    void _generateBRDFMap();

public:
    Skybox(int windowWidth, int windowHeight, const char* fileName = "Textures/skybox/village_cloudy_sky_dome_4k.hdr");

	void loadEquirectangularMap(const char* file_name);
    void renderSkybox();

    GLuint getIrradianceMap() const { return irradianceMap; }
    GLuint getBRDFTexture() const { return brdfTexture; };
    GLuint getPrefilterTexture() const { return prefilterMap; }

    ~Skybox() {
        if (FBO != 0)
            glDeleteFramebuffers(1, &FBO);

        if (RBO != 0)
            glDeleteRenderbuffers(1, &RBO);

        if (environmentMap != 0)
            glDeleteTextures(1, &environmentMap);

        if (irradianceMap != 0)
            glDeleteTextures(1, &irradianceMap);

        if (prefilterMap != 0)
            glDeleteTextures(1, &prefilterMap);

        if (brdfTexture != 0)
            glDeleteTextures(1, &brdfTexture);
    }
};

