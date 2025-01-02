#pragma once
#include <vector>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Geometry.hpp"
#include <glimac/common.hpp>
#include <glimac/getTime.hpp>

using Vertex = Geometry::Vertex;

// Représente un cylindre discretisé centré en (0, 0, 0) (dans son repère local)
// Son axe vertical est (0, 1, 0) et ses axes transversaux sont (1, 0, 0) et (0, 0, 1)
class Cylinder : public Geometry
{
    // Alloue et construit les données (implantation dans le .cpp)
    void build(GLfloat r, GLfloat height, GLsizei discLat, GLsizei discLong)
    {
        GLfloat rcpLat = 1.f / discLat, rcpLong = 1.f / discLong;
        GLfloat dPhi = 2 * glm::pi<float>() * rcpLat, dHeight = height * rcpLong;

        std::vector<Vertex> data;

        for (GLsizei j = 0; j <= discLong; ++j)
        {
            GLfloat z = -height / 2 + j * dHeight;

            for (GLsizei i = 0; i <= discLat; ++i)
            {
                Vertex vertex;

                // Coordonnées de texture
                vertex.m_TexCoords.x = i * rcpLat;
                vertex.m_TexCoords.y = 1.f - j * rcpLong;

                // Normales (unité) de la surface du cylindre
                vertex.m_Normal.x = cos(i * dPhi);
                vertex.m_Normal.y = 0.f; // Pas de composante verticale
                vertex.m_Normal.z = sin(i * dPhi);

                // Position (le long de l'axe Z et autour du cercle)
                vertex.m_Position.x = r * vertex.m_Normal.x;
                vertex.m_Position.y = z;
                vertex.m_Position.z = r * vertex.m_Normal.z;

                data.push_back(vertex);
            }
        }

        m_nVertexCount = discLat * discLong * 6;

        GLuint idx = 0;
        // Construit les vertex finaux en regroupant les données en triangles:
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

        // Init VBO
        glGenBuffers(1, &this->_vbo);
        glBindBuffer(GL_ARRAY_BUFFER, this->_vbo);
        glBufferData(GL_ARRAY_BUFFER, this->m_VertexBuffer.size() * sizeof(Geometry::Vertex), this->getVertexBuffer(), GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    void initMesh()
    {
        int idMat = m_Materials.size() == 0 ? -1 : 0;

        size_t lastMeshIndex = getLastMeshIndex();
        size_t newIndex = lastMeshIndex + this->m_VertexBuffer.size();
        Geometry::Mesh mesh("cylinder", lastMeshIndex, newIndex, idMat);
        updateLastMeshIndex(newIndex);

        const GLuint VERTEX_ATTR_POSITION = 1;
        const GLuint VERTEX_ATTR_NORMAL = 2;
        const GLuint VERTEX_ATTR_TEX = 3;

        glGenVertexArrays(1, &mesh.vao);
        glBindVertexArray(mesh.vao);
        glBindBuffer(GL_ARRAY_BUFFER, this->_vbo);

        glEnableVertexAttribArray(VERTEX_ATTR_POSITION);
        glEnableVertexAttribArray(VERTEX_ATTR_NORMAL);
        glEnableVertexAttribArray(VERTEX_ATTR_TEX);

        glVertexAttribPointer(VERTEX_ATTR_POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(Geometry::Vertex), offsetof(Geometry::Vertex, m_Position));
        glVertexAttribPointer(VERTEX_ATTR_NORMAL, 3, GL_FLOAT, GL_FALSE, sizeof(Geometry::Vertex), (GLvoid *)offsetof(Geometry::Vertex, m_Normal));
        glVertexAttribPointer(VERTEX_ATTR_TEX, 3, GL_FLOAT, GL_FALSE, sizeof(Geometry::Vertex), (GLvoid *)offsetof(Geometry::Vertex, m_TexCoords));

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        this->m_MeshBuffer.emplace_back(std::move(mesh));
    }

public:
    Cylinder()
    {
    }

    Cylinder(GLfloat radius, GLfloat height, GLsizei discLat, GLsizei discLong) : m_nVertexCount(0)
    {
        build(radius, height, discLat, discLong);
        initMesh();
    }

    Cylinder(GLfloat radius, GLfloat height, GLsizei discLat, GLsizei discLong, Geometry::Material material) : m_nVertexCount(0)
    {
        m_Materials.push_back(material);
        build(radius, height, discLat, discLong);
        initMesh();
    }

    void initCylinder(GLfloat radius, GLfloat height, GLsizei discLat, GLsizei discLong, Geometry::Material cylMat)
    {
        m_Materials.push_back(cylMat);
        build(radius, height, discLat, discLong);
        initMesh();
    }

    const Geometry::Vertex *getDataPointer() const
    {
        return &this->m_VertexBuffer[0];
    }

    GLsizei getVertexCount() const
    {
        return m_nVertexCount;
    }

    void animateCylinder()
    {
        auto time = glimac::getTime();

        float waveFrequency = 2.0f; // Fréquence des vagues
        float waveAmplitude = 0.5f; // Amplitude des vagues

        for (auto &vertex : m_VertexBuffer)
        {
            float waveFactor = sin(vertex.m_Position.y * waveFrequency + time) * waveAmplitude;
            vertex.m_Position.y += waveFactor;
            vertex.m_Position.x += waveFactor;
        }

        glBindBuffer(GL_ARRAY_BUFFER, this->_vbo);
        glBufferData(GL_ARRAY_BUFFER, this->m_VertexBuffer.size() * sizeof(Geometry::Vertex), this->getVertexBuffer(), GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    void elasticDeformation()
    {
        auto time = glimac::getTime();

        float amplitude = 0.3f;
        float frequency = 0.5f;

        for (auto &vertex : m_VertexBuffer)
        {
            float distance = glm::length(vertex.m_Position);
            float elasticFactor = amplitude * sin(frequency * distance + time);

            vertex.m_Position.x += elasticFactor * vertex.m_Normal.x;
            vertex.m_Position.y += elasticFactor * vertex.m_Normal.y;
            vertex.m_Position.z += elasticFactor * vertex.m_Normal.z;
        }

        // Mettre à jour les données du VBO après la déformation
        glBindBuffer(GL_ARRAY_BUFFER, this->_vbo);
        glBufferData(GL_ARRAY_BUFFER, this->m_VertexBuffer.size() * sizeof(Geometry::Vertex), this->getVertexBuffer(), GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

private:
    GLsizei m_nVertexCount;
};
