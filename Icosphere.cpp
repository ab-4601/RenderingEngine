#include "Icosphere.h"

Icosphere::Icosphere()
    : Mesh() {
    this->_generateIcosohedron();
}

void Icosphere::addVertex(float x, float y, float z) {
    this->vertices.push_back(x);
    this->vertices.push_back(y);
    this->vertices.push_back(z);
}

void Icosphere::addTempVertices(float x, float y, float z) {
    this->tempVertices.push_back(x);
    this->tempVertices.push_back(y);
    this->tempVertices.push_back(z);
}

void Icosphere::addTempIndices(unsigned int a, unsigned int b, unsigned int c) {
    this->tempIndices.push_back(a);
    this->tempIndices.push_back(b);
    this->tempIndices.push_back(c);
}

void Icosphere::addTexCoord(glm::vec2 uvCoord) {
    this->texCoords.push_back(uvCoord.x);
    this->texCoords.push_back(uvCoord.y);
}

void Icosphere::_generateIcosohedron() {
    const float X = 0.525731112119133606f;
    const float Z = 0.850650808352039932f;
    const float N = 0.f;

    this->vertices = {
        -X,N,Z, X,N,Z, -X,N,-Z,  X,N,-Z,
        N,Z,X,  N,Z,-X, N,-Z,X,  N,-Z,-X,
        Z,X,N, -Z,X,N,  Z,-X,N, -Z,-X,N
    };

    this->indices = {
        0,4,1,   0,9,4,   9,5,4,   4,5,8,   4,8,1,
        8,10,1,  8,3,10,  5,3,8,   5,2,3,   2,7,3,
        7,10,3,  7,6,10,  7,11,6,  11,0,6,  0,1,6,
        6,1,10,  9,0,11,  9,11,2,  9,2,5,   7,2,11
    };
}

glm::vec2 Icosphere::getTexCoord(glm::vec3 point) {
    point = glm::normalize(point);

    float theta = atan2f(point.z, point.x);
    float phi = acosf(point.y);

    float u = static_cast<float>((theta + PI) / (2 * PI));
    float v = static_cast<float>(phi / PI);

    return glm::vec2(u, v);
}

void Icosphere::_generateTexCoords() {
    glm::vec3 point{};

    for (size_t i = 0; i < this->vertices.size(); i += 3) {
        point.x = vertices.at(i);
        point.y = vertices.at(i + 1);
        point.z = vertices.at(i + 2);

        glm::vec2 uvCoord = this->getTexCoord(point);

        this->addTexCoord(uvCoord);
    }
}

void Icosphere::_subdivide(glm::vec3 a, glm::vec3 b, glm::vec3 c, unsigned int index1, unsigned int index2, unsigned int index3) {
    glm::vec3 mid1 = (a + b) / 2.f;
    mid1 = glm::normalize(mid1);

    glm::vec3 mid2 = (b + c) / 2.f;
    mid2 = glm::normalize(mid2);

    glm::vec3 mid3 = (a + c) / 2.f;
    mid3 = glm::normalize(mid3);

    this->addVertex(mid1.x, mid1.y, mid1.z);
    unsigned int newIndex1 = (unsigned int)(this->vertices.size() / 3) - 1;

    this->addVertex(mid2.x, mid2.y, mid2.z);
    unsigned int newIndex2 = (unsigned int)(this->vertices.size() / 3) - 1;

    this->addVertex(mid3.x, mid3.y, mid3.z);
    unsigned int newIndex3 = (unsigned int)(this->vertices.size() / 3) - 1;

    this->addTempIndices(newIndex1, newIndex2, newIndex3);
    this->addTempIndices(index1, newIndex1, newIndex3);
    this->addTempIndices(newIndex1, index2, newIndex2);
    this->addTempIndices(newIndex2, index3, newIndex3);
}

void Icosphere::smoothSphere(int subdivisions) {
    unsigned int index1{ 0 }, index2{ 0 }, index3{ 0 };

    while (subdivisions != 0) {
        std::vector<unsigned int> tmpIndices = this->indices;
        std::vector<float> tmpVertices = this->vertices;

        for (size_t i = 0; i < tmpIndices.size(); i += 3) {
            index1 = tmpIndices.at(i);
            index2 = tmpIndices.at(i + 1);
            index3 = tmpIndices.at(i + 2);

            float x1 = tmpVertices.at(index1 * 3);
            float y1 = tmpVertices.at((index1 * 3) + 1);
            float z1 = tmpVertices.at((index1 * 3) + 2);

            glm::vec3 a(x1, y1, z1);

            float x2 = tmpVertices.at(index2 * 3);
            float y2 = tmpVertices.at((index2 * 3) + 1);
            float z2 = tmpVertices.at((index2 * 3) + 2);

            glm::vec3 b(x2, y2, z2);

            float x3 = tmpVertices.at(index3 * 3);
            float y3 = tmpVertices.at((index3 * 3) + 1);
            float z3 = tmpVertices.at((index3 * 3) + 2);

            glm::vec3 c(x3, y3, z3);

            this->_subdivide(a, b, c, index1, index2, index3);
        }

        this->indices = this->tempIndices;
        this->tempIndices.clear();

        --subdivisions;
    }

    this->normals = this->vertices;

    this->_generateTexCoords();
}