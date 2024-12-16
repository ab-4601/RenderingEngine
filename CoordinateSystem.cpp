#include "CoordinateSystem.h"

CoordinateSystem::CoordinateSystem() {
    this->axes.clear();
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

    this->axes.push_back(X);
    this->axes.push_back(XNegative);

    this->axes.push_back(Y);
    this->axes.push_back(YNegative);

    this->axes.push_back(Z);
    this->axes.push_back(ZNegative);
}

void CoordinateSystem::drawCoordinateSystem(GLint windowHeight, GLint windowWidth, GLint bufferWidth,
    GLint bufferHeight, const Camera& camera) 
{
    this->shader.useShader();

    this->model = glm::mat4(1.f);

    this->model = glm::scale(this->model, glm::vec3(INT_MAX));
    this->shader.setMat4("model", this->model);

    glLineWidth(5.f);

    for (const auto& elem : this->axes) {
        this->shader.setVec3("vColor", elem->getColor());
        elem->renderMesh();
    }

    this->model = glm::mat4(1.f);

    glm::vec3 axisPosition = camera.getCameraPosition() + glm::normalize(camera.getCameraLookDirection()) * 100.f;
    this->model = glm::translate(this->model, axisPosition);
    this->model = glm::scale(this->model, glm::vec3(200.f));

    this->shader.setMat4("model", this->model);

    glViewport(windowWidth / 50, windowHeight / 50, bufferWidth / 12, bufferHeight / 12);

    glDisable(GL_DEPTH_TEST);
    glLineWidth(3.f);
    for (const auto& elem : this->axes) {
        this->shader.setVec3("vColor", elem->getColor());
        elem->renderMesh();
    }
    glEnable(GL_DEPTH_TEST);

    glViewport(0, 0, bufferWidth, bufferHeight);

    glLineWidth((GLfloat)1.f);

    this->shader.endShader();
}

CoordinateSystem::~CoordinateSystem() {
    for (auto& elem : this->axes)
        delete elem;
}