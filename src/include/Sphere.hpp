#pragma once
#include <vector>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Geometry.hpp"
#include <glimac/common.hpp>

using Vertex = Geometry::Vertex;

// Représente une sphère discrétisée centrée en (0, 0, 0) (dans son repère local)
// Son axe vertical est (0, 1, 0) et ses axes transversaux sont (1, 0, 0) et (0, 0, 1)
class Sphere : public Geometry
{
    // Alloue et construit les données (implantation dans le .cpp)
    void build(GLfloat r, GLsizei discLat, GLsizei discLong)
    {
        // Equation paramétrique en (r, phi, theta) de la sphère
        // avec r >= 0, -PI / 2 <= theta <= PI / 2, 0 <= phi <= 2PI
        //
        // x(r, phi, theta) = r sin(phi) cos(theta)
        // y(r, phi, theta) = r sin(theta)
        // z(r, phi, theta) = r cos(phi) cos(theta)
        //
        // Discrétisation:
        // dPhi = 2PI / discLat, dTheta = PI / discLong
        //
        // x(r, i, j) = r * sin(i * dPhi) * cos(-PI / 2 + j * dTheta)
        // y(r, i, j) = r * sin(-PI / 2 + j * dTheta)
        // z(r, i, j) = r * cos(i * dPhi) * cos(-PI / 2 + j * dTheta)

        GLfloat rcpLat = 1.f / discLat, rcpLong = 1.f / discLong;
        GLfloat dPhi = 2 * glm::pi<float>() * rcpLat, dTheta = glm::pi<float>() * rcpLong;

        std::vector<Vertex> data;

        // Construit l'ensemble des vertex
        for (GLsizei j = 0; j <= discLong; ++j)
        {
            GLfloat cosTheta = cos(-glm::pi<float>() / 2 + j * dTheta);
            GLfloat sinTheta = sin(-glm::pi<float>() / 2 + j * dTheta);

            for (GLsizei i = 0; i <= discLat; ++i)
            {
                Vertex vertex;

                vertex.m_TexCoords.x = i * rcpLat;
                vertex.m_TexCoords.y = 1.f - j * rcpLong;

                vertex.m_Normal.x = sin(i * dPhi) * cosTheta;
                vertex.m_Normal.y = sinTheta;
                vertex.m_Normal.z = cos(i * dPhi) * cosTheta;

                vertex.m_Position = r * vertex.m_Normal;

                data.push_back(vertex);
            }
        }

        m_nVertexCount = discLat * discLong * 6;

        GLuint idx = 0;
        // Construit les vertex finaux en regroupant les données en triangles:
        // Pour une longitude donnée, les deux triangles formant une face sont de la forme:
        // (i, i + 1, i + discLat + 1), (i, i + discLat + 1, i + discLat)
        // avec i sur la bande correspondant à la longitude
        for (GLsizei j = 0; j < discLong; ++j)
        {
            GLsizei offset = j * (discLat + 1);
            for (GLsizei i = 0; i < discLat; ++i)
            {
                m_VertexBuffer.push_back(data[offset + i]);
                m_VertexBuffer.push_back(data[offset + (i + 1)]);
                m_VertexBuffer.push_back(data[offset + discLat + 1 + (i + 1)]);
                m_VertexBuffer.push_back(data[offset + i]);
                m_VertexBuffer.push_back(data[offset + discLat + 1 + (i + 1)]);
                m_VertexBuffer.push_back(data[offset + i + discLat + 1]);
            }
        }

        // Attention ! dans cette implantation on duplique beaucoup de sommets. Une meilleur stratégie est de passer
        // par un Index Buffer Object, que nous verrons dans les prochains TDs
    }

public:
    // Constructeur: alloue le tableau de données et construit les attributs des vertex
    Sphere(GLfloat radius, GLsizei discLat, GLsizei discLong) : m_nVertexCount(0)
    {
        build(radius, discLat, discLong); // Construction (voir le .cpp)
    }

    // Renvoit le pointeur vers les données
    const Geometry::Vertex *getDataPointer() const
    {
        return &this->m_VertexBuffer[0];
    }

    // Renvoit le nombre de vertex
    GLsizei getVertexCount() const
    {
        return m_nVertexCount;
    }

private:
    GLsizei m_nVertexCount; // Nombre de sommets
};