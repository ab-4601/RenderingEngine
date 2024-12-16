#pragma once

#include "Window.h"
#include "Camera.h"
#include "Mesh.h"
#include "Shader.h"
#include "SelectionTexture.h"

class MouseSelector {
private:
	Shader shader{ "selection.vert", "selection.frag" };
	SelectionTexture pixelInfo;

public:
	MouseSelector(unsigned int windowWidth, unsigned int windowHeight);

	void pickingPhase(std::vector<Mesh*>& meshes, GLuint framebuffer = 0);
	int mouseSelectionResult(int windowHeight, int x, int y);

	~MouseSelector() = default;
};