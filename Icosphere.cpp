#include "Icosphere.h"

Icosphere::Icosphere()
    : Mesh() {
    this->tempIndices.clear();
    this->_generateIcosahedron();
}

void Icosphere::_generateIcosahedron() {
    const float X = 0.525731112119133606f;
    const float Z = 0.850650808352039932f;
    const float N = 0.f;

    this->vertices = {
        Vertex({-X,N,Z}, {0.f, 0.f}, {-X,N,Z}), Vertex({X,N,Z}, {0.f, 0.f}, {X,N,Z}),
        Vertex({-X,N,-Z}, {0.f, 0.f}, {-X,N,-Z}),  Vertex({X,N,-Z}, {0.f, 0.f}, {X,N,-Z}),
        Vertex({N,Z,X}, {0.f, 0.f}, {N,Z,X}),  Vertex({N,Z,-X}, {0.f, 0.f}, {N,Z,-X}),
        Vertex({N,-Z,X}, {0.f, 0.f}, {N,-Z,X}),  Vertex({N,-Z,-X}, {0.f, 0.f}, {N,-Z,-X}),
        Vertex({Z,X,N}, {0.f, 0.f}, {Z,X,N}), Vertex({-Z,X,N}, {0.f, 0.f}, {-Z,X,N}),
        Vertex({Z,-X,N}, {0.f, 0.f}, {Z,-X,N}), Vertex({-Z,-X,N}, {0.f, 0.f}, {-Z,-X,N})
    };

    this->indices = {
        0,4,1,   0,9,4,   9,5,4,   4,5,8,   4,8,1,
        8,10,1,  8,3,10,  5,3,8,   5,2,3,   2,7,3,
        7,10,3,  7,6,10,  7,11,6,  11,0,6,  0,1,6,
        6,1,10,  9,0,11,  9,11,2,  9,2,5,   7,2,11
    };
}

void Icosphere::_generateTexCoords() {
    glm::vec3 point{ 0.f };
    float theta{ 0.f }, phi{ 0.f }, u{ 0.f }, v{ 0.f };
    for (size_t i = 0; i < this->vertices.size(); i++) {
        point = glm::normalize(this->vertices.at(i).position);

        theta = atan2f(point.z, point.x);
        phi = acosf(point.y);

        u = ((theta + PI) / (2 * PI));
        v = (phi / PI);

        this->vertices.at(i).texel = glm::vec2(u, v);
    }
}

void Icosphere::_subdivide(glm::vec3 a, glm::vec3 b, glm::vec3 c, unsigned int index1, unsigned int index2,
    unsigned int index3) 
{
    glm::vec3 mid1 = (a + b) / 2.f;
    mid1 = glm::normalize(mid1);

    glm::vec3 mid2 = (b + c) / 2.f;
    mid2 = glm::normalize(mid2);

    glm::vec3 mid3 = (a + c) / 2.f;
    mid3 = glm::normalize(mid3);

    glm::vec3 position{ mid1.x, mid1.y, mid1.z };
    this->addVertex(position, glm::vec2(0.f), position);
    unsigned int newIndex1 = (unsigned int)(this->vertices.size() - 1);

    position = glm::vec3(mid2.x, mid2.y, mid2.z);
    this->addVertex(position, glm::vec2(0.f), position);
    unsigned int newIndex2 = (unsigned int)(this->vertices.size() - 1);

    position = glm::vec3(mid3.x, mid3.y, mid3.z);
    this->addVertex(position, glm::vec2(0.f), position);
    unsigned int newIndex3 = (unsigned int)(this->vertices.size() - 1);

    this->addTempIndices(newIndex1, newIndex2, newIndex3);
    this->addTempIndices(index1, newIndex1, newIndex3);
    this->addTempIndices(newIndex1, index2, newIndex2);
    this->addTempIndices(newIndex2, index3, newIndex3);
}

void Icosphere::smoothSphere(int subdivisions) {
    unsigned int index1{ 0 }, index2{ 0 }, index3{ 0 };

    std::vector<unsigned int> tmpIndices{};
    std::vector<Vertex> tmpVertices{};

    while (subdivisions != 0) {
        tmpIndices = this->indices;
        tmpVertices = this->vertices;

        for (size_t i = 0; i < tmpIndices.size(); i += 3) {
            index1 = tmpIndices.at(i);
            index2 = tmpIndices.at(i + 1);
            index3 = tmpIndices.at(i + 2);

            glm::vec3 a{ tmpVertices.at(index1).position };

            glm::vec3 b{ tmpVertices.at(index2).position };

            glm::vec3 c{ tmpVertices.at(index3).position };

            this->_subdivide(a, b, c, index1, index2, index3);
        }

        this->indices = this->tempIndices;
        this->tempIndices.clear();

        --subdivisions;
    }

    this->_generateTexCoords();
}