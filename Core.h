#ifndef _CORE_H_
#define _CORE_H_

enum CONTEXT_VERSION { MAJOR = 4, MINOR = 6 };

typedef unsigned int uint;

const uint MAX_POINT_LIGHTS = 3;
const uint MAX_SPOT_LIGHTS = 3;
const uint MAX_CASCADES = 16;
const uint samples = 4;

const float PI = 3.14159265f;
const float GRAVITY = -10.f;
const float toRadians = (float)PI / 180;

const float near_plane = 3.f;
const float far_plane = 7000.f;

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <random>
#include <unordered_map>
#include <algorithm>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <ImGuizmo.h>

#include <GL/glew.h>
#include <GL/GL.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_access.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#endif