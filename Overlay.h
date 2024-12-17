#pragma once

#include "Core.h"
#include "Mesh.h"
#include "Camera.h"
#include "Window.h"

class Overlay {
private:
	ImGuizmo::OPERATION transformOperation;
	ImFont* mainfont;

	float colorBuffer[4] = { 0.f, 0.f, 0.f, 1.f };
	float translation[3] = { 0.f, 0.f, 0.f };
	float rotation[3] = { 0.f, 0.f, 0.f };
	float scale[3] = { 0.f, 0.f, 0.f };
	float metallic = 0.f;
	float ao = 0.f;
	float roughness = 0.f;

	void DrawVec3Control(const std::string& label, float* values, float resetValue = 0.f, float columnWidth = 120.f);
	void DrawDragFloat(const std::string& label, float* values, float speed = 1.f, float columnWidth = 120.f);
	void DrawSliderFloat(const std::string& label, float* values, float min, float max, float columnWidth = 120.f);

public:
	Overlay() : transformOperation{ ImGuizmo::TRANSLATE }, mainfont{ nullptr } {};

	ImGuiIO& _init(GLFWwindow* window);

	void _newFrame();
	void _updateTransformOperation(const Window& window);
	void renderGUI(
		ImGuiIO& io, float& exposure, float& shadowRadius, float& filterRadius, float &bloomThreshold,
		bool& drawSkybox, bool& displayGrid, bool& displayCoordinateSystem, bool& enableBloom, bool& enableWireframe, 
		bool& enableShadows, bool& enableHDR, bool& enableSSAO, glm::vec3& dirLightLocation, Mesh* currMesh = nullptr
	);
	void manipulate(int bufferWidth, int bufferHeight, const Camera& camera, Mesh* mesh);

	~Overlay();
};

