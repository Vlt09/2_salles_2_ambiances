#pragma once
#include "Cube.hpp"
#include "Quad.hpp"
#include "Cone.hpp"

struct TwistCube
{
    Cube _cube; // Cube central
    // std::vector<Cone> _cones; // Tableau dynamique de 8 cônes
    Cone _cone;

    // Constructeur de TwistCube
    TwistCube(int size, GLfloat coneHeight, GLfloat coneRadius, GLsizei coneDiscLat, GLsizei coneDiscHeight)
        : _cube(size), _cone(coneHeight, coneRadius, coneDiscLat, coneDiscHeight) // Initialise le cube et le tableau de 8 cônes
    {
        std::cout << "Cube vertex count: " << _cube.getVertexCount() << std::endl;

        // Calcul des positions des 8 coins du cube
        GLfloat halfSize = size / 2.0f;
        glm::vec3 cornerPositions[8] = {
            glm::vec3(-halfSize, -halfSize, -halfSize), // Coin 0
            glm::vec3(halfSize, -halfSize, -halfSize),  // Coin 1
            glm::vec3(-halfSize, -halfSize, halfSize),  // Coin 2
            glm::vec3(halfSize, -halfSize, halfSize),   // Coin 3
            glm::vec3(-halfSize, halfSize, -halfSize),  // Coin 4
            glm::vec3(halfSize, halfSize, -halfSize),   // Coin 5
            glm::vec3(-halfSize, halfSize, halfSize),   // Coin 6
            glm::vec3(halfSize, halfSize, halfSize)     // Coin 7
        };

        // Initialise chaque cône aux positions des coins
        // for (int i = 0; i < 8; ++i)
        // {
        //     _cones[i] = Cone(coneHeight, coneRadius, coneDiscLat, coneDiscHeight);
        //     // _cones[i].translateModel(cornerPositions[i]);
        // }
    }

    // Fonction pour mettre à jour les positions des cônes
    void updateCone()
    {
        // Calcul des positions des 8 coins du cube (si nécessaire pour mise à jour)
        // GLfloat halfSize = 5. / 2.0f;
        // glm::vec3 cornerPositions[8] = {
        //     glm::vec3(-halfSize, -halfSize, -halfSize), // Coin 0
        //     glm::vec3(halfSize, -halfSize, -halfSize),  // Coin 1
        //     glm::vec3(-halfSize, -halfSize, halfSize),  // Coin 2
        //     glm::vec3(halfSize, -halfSize, halfSize),   // Coin 3
        //     glm::vec3(-halfSize, halfSize, -halfSize),  // Coin 4
        //     glm::vec3(halfSize, halfSize, -halfSize),   // Coin 5
        //     glm::vec3(-halfSize, halfSize, halfSize),   // Coin 6
        //     glm::vec3(halfSize, halfSize, halfSize)     // Coin 7
        // };

        // // Mettre à jour la position de chaque cône
        // for (int i = 0; i < 8; ++i)
        // {
        //     std::cout << "Updating cone " << i << " to position " << cornerPositions[i].x << ", "
        //               << cornerPositions[i].y << ", " << cornerPositions[i].z << std::endl;
        //     _cones[i].translateModel(cornerPositions[i]);
        // }
    }
};

struct Helix
{
    Geometry _helix;

    Helix(int size)
    {
        float yOffset = 1.5f;
        float xOffset = 1.0f;

        glm::vec3 offSet = glm::vec3(0., 0., 0.);
        glm::mat4 prevMat = glm::mat4(1);
        auto vertice = Quad::QuadVertices(1, 1);
        for (int i = 0; i < size; i++)
        {
            auto mesh = _helix.addFromVertices(vertice, -1);
            mesh._transform = glm::translate(prevMat, offSet);

            prevMat = mesh._transform;
            offSet.x += xOffset;
            offSet.y += yOffset;
        }
        _helix.initMeshData();
    }
};

// struct VortexEffect
// {
//     Cylinder _vortex;     // Pointeur vers l'objet géométrique
//     float vortexStrength; // Force du vortex

//     VortexEffect(float strength = 1.0f, GLfloat radius, GLfloat height, GLsizei discLat, GLsizei discLong)
//         : vortexStrength(strength), _vortex(radius, height, discLat, discLong)
//     {
//     }

//     void applyVortexEffect()
//     {
//         auto time = glimac::getTime();

//         auto data = _vortex.getDataPointer();
//         auto size = _vortex.getVertexCount();
//         for (auto i = 0; i < size; i++)
//         {
//             float angle = vortexStrength * data[i].m_Position.y + time;

//             float cosTheta = cos(angle);
//             float sinTheta = sin(angle);

//             // Effectuer la rotation autour de l'axe Z
//             float newX = cosTheta * data[i].m_Position.x - sinTheta * data[i].m_Position.z;
//             float newZ = sinTheta * data[i].m_Position.x + cosTheta * data[i].m_Position.z;

//             data[i].m_Position = glm::vec3;
//             data[i].m_Position.z = newZ;
//         }

//         glBindBuffer(GL_ARRAY_BUFFER, _vortex.getMeshBuffer()[0].vbo);
//         glBufferData(GL_ARRAY_BUFFER, size * sizeof(Geometry::Vertex), data, GL_STATIC_DRAW);
//         glBindBuffer(GL_ARRAY_BUFFER, 0);
//     }

//     void setVortexStrength(float strength)
//     {
//         vortexStrength = strength;
//     }
// };