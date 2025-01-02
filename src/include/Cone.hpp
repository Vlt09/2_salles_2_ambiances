#pragma once

#include "Geometry.hpp"
#include <vector>
#include <glimac/common.hpp>

// Représente un cone ouvert discrétisé dont la base est centrée en (0, 0, 0) (dans son repère local)
// Son axe vertical est (0, 1, 0) et ses axes transversaux sont (1, 0, 0) et (0, 0, 1)
class Cone : public Geometry
{

public:
    static std::vector<Geometry::Vertex> ConeVertices(GLfloat height, GLfloat r, GLsizei discLat, GLsizei discHeight)
    {
        GLfloat rcpLat = 1.f / discLat, rcpH = 1.f / discHeight;
        GLfloat dPhi = 2 * glm::pi<float>() * rcpLat, dH = height * rcpH;

        std::vector<Geometry::Vertex> data;

        // Construit l'ensemble des vertex
        for (GLsizei j = 0; j <= discHeight; ++j)
        {
            for (GLsizei i = 0; i < discLat; ++i)
            {
                Geometry::Vertex vertex;

                vertex.m_TexCoords.x = i * rcpLat;
                vertex.m_TexCoords.y = j * rcpH;

                vertex.m_Position.x = r * (height - j * dH) * sin(i * dPhi) / height;
                vertex.m_Position.y = j * dH;
                vertex.m_Position.z = r * (height - j * dH) * cos(i * dPhi) / height;

                /* avec cette formule la normale est mal définie au sommet (= (0, 0, 0))
                vertex.normal.x = vertex.position.x;
                vertex.normal.y = r * r * (1 - vertex.position.y / height) / height;
                vertex.normal.z = vertex.position.z;
                vertex.normal = glm::normalize(vertex.normal);
                */

                vertex.m_Normal.x = sin(i * dPhi);
                vertex.m_Normal.y = r / height;
                vertex.m_Normal.z = cos(i * dPhi);
                vertex.m_Normal = glm::normalize(vertex.m_Normal);

                data.push_back(vertex);
            }
        }
        return data;
    }

    // Alloue et construit les données (implantation dans le .cpp)
    void build(GLfloat height, GLfloat radius, GLsizei discLat, GLsizei discHeight);

    Cone() {};

    // Constructeur: alloue le tableau de données et construit les attributs des vertex
    Cone(GLfloat height, GLfloat radius, GLsizei discLat, GLsizei discHeight)
    {
        build(height, radius, discLat, discHeight); // Construction (voir le .cpp)
        initMesh();
    }

    void initMesh();
};
