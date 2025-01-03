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
