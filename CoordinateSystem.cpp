#include "CoordinateSystem.h"

CoordinateSystem::CoordinateSystem() {
    axes.clear();
}

void CoordinateSystem::createCoordinateSystem() {
    glm::vec3 RED(1.f, 0.f, 0.f), GREEN(0.f, 1.f, 0.f), BLUE(0.f, 0.1f, 1.f);
    glm::vec3 origin(0.f, 0.f, 0.f);

    Line* X = new Line(glm::vec3(0.5f, 0.f, 0.f), origin, RED);
    Line* Y = new Line(glm::vec3(0.f, 0.5f, 0.f), origin, GREEN);
    Line* Z = new Line(glm::vec3(0.f, 0.f, 0.5f), origin, BLUE);

    Line* XNegative = new Line(glm::vec3(-0.5f, 0.f, 0.f), origin, RED);
    Line* YNegative = new Line(glm::vec3(0.f, -0.5f, 0.f), origin, GREEN);
    Line* ZNegative = new Line(glm::vec3(0.f, 0.f, -0.5f), origin, BLUE);

    X->createMesh();
    Y->createMesh();
    Z->createMesh();

    XNegative->createMesh();
    YNegative->createMesh();
    ZNegative->createMesh();

    axes.push_back(X);
    axes.push_back(XNegative);

    axes.push_back(Y);
    axes.push_back(YNegative);

    axes.push_back(Z);
    axes.push_back(ZNegative);
}

void CoordinateSystem::drawCoordinateSystem(GLint windowHeight, GLint windowWidth, GLint bufferWidth,
    GLint bufferHeight, const Camera& camera) 
{
    shader.useShader();

    model = glm::mat4(1.f);

    model = glm::scale(model, glm::vec3((float)INT_MAX));
    shader.setMat4("model", model);

    glLineWidth(5.f);

    for (const auto& elem : axes) {
        shader.setVec3("vColor", elem->getColor());
        elem->renderMesh();
    }

    model = glm::mat4(1.f);

    glm::vec3 axisPosition = camera.getCameraPosition() + glm::normalize(camera.getCameraLookDirection()) * 100.f;
    model = glm::translate(model, axisPosition);
    model = glm::scale(model, glm::vec3(200.f));

    shader.setMat4("model", model);

    glViewport(windowWidth / 50, windowHeight / 50, bufferWidth / 12, bufferHeight / 12);

    glDisable(GL_DEPTH_TEST);
    glLineWidth(3.f);
    for (const auto& elem : axes) {
        shader.setVec3("vColor", elem->getColor());
        elem->renderMesh();
    }
    glEnable(GL_DEPTH_TEST);

    glViewport(0, 0, bufferWidth, bufferHeight);

    glLineWidth((GLfloat)1.f);

    shader.endShader();
}

CoordinateSystem::~CoordinateSystem() {
    for (auto& elem : axes)
        delete elem;
}