#pragma once

#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "Shader.h"
#include "Camera.h"

struct LightMesh {
    GLuint VAO{ 0 }, VBO{ 0 };

	std::vector<GLfloat> vertices = {
        -1.f, -1.f, -1.f,
         1.f, -1.f, -1.f,
         1.f,  1.f, -1.f,
         1.f,  1.f, -1.f,
        -1.f,  1.f, -1.f,
        -1.f, -1.f, -1.f,

        -1.f, -1.f,  1.f,  
         1.f, -1.f,  1.f,  
         1.f,  1.f,  1.f,  
         1.f,  1.f,  1.f,  
        -1.f,  1.f,  1.f,  
        -1.f, -1.f,  1.f,  

        -1.f,  1.f,  1.f,
        -1.f,  1.f, -1.f,
        -1.f, -1.f, -1.f,
        -1.f, -1.f, -1.f,
        -1.f, -1.f,  1.f,
        -1.f,  1.f,  1.f,

         1.f,  1.f,  1.f, 
         1.f,  1.f, -1.f, 
         1.f, -1.f, -1.f, 
         1.f, -1.f, -1.f, 
         1.f, -1.f,  1.f, 
         1.f,  1.f,  1.f, 

        -1.f, -1.f, -1.f, 
         1.f, -1.f, -1.f, 
         1.f, -1.f,  1.f, 
         1.f, -1.f,  1.f, 
        -1.f, -1.f,  1.f, 
        -1.f, -1.f, -1.f, 

        -1.f,  1.f, -1.f,  
         1.f,  1.f, -1.f,  
         1.f,  1.f,  1.f,  
         1.f,  1.f,  1.f,  
        -1.f,  1.f,  1.f,  
        -1.f,  1.f, -1.f,
	};

    void createMesh() {
        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);

        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    void renderMesh() const {
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);

        glDrawArrays(GL_TRIANGLES, 0, (GLsizei)vertices.size());

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }
};

class LightSources {
private:
    Shader shader{ "light.vert", "light.frag" };
	LightMesh sourceMesh;

public:
	LightSources();

	void renderLightSources(DirectionalLight& directionalLight, std::vector<PointLight>& pointLights,
        std::vector<SpotLight>& spotLights, int pointLightCount, int spotLightCount);

	~LightSources() = default;
};

