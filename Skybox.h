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
        glBindVertexArray(this->VAO);
        glBindBuffer(GL_ARRAY_BUFFER, this->VBO);

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

    GLuint getIrradianceMap() const { return this->irradianceMap; }
    GLuint getBRDFTexture() const { return this->brdfTexture; };
    GLuint getPrefilterTexture() const { return this->prefilterMap; }

    ~Skybox() {
        if (this->FBO != 0)
            glDeleteFramebuffers(1, &this->FBO);

        if (this->RBO != 0)
            glDeleteRenderbuffers(1, &this->RBO);

        if (this->environmentMap != 0)
            glDeleteTextures(1, &this->environmentMap);

        if (this->irradianceMap != 0)
            glDeleteTextures(1, &this->irradianceMap);

        if (this->prefilterMap != 0)
            glDeleteTextures(1, &this->prefilterMap);

        if (this->brdfTexture != 0)
            glDeleteTextures(1, &this->brdfTexture);
    }
};

