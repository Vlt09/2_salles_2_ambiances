#pragma once
#include "Geometry.hpp"
#include <cmath>
#include <vector>

class Ring : public Geometry
{
private:
    void build(GLfloat outerRadius, GLfloat innerRadius, unsigned int sides, unsigned int rings)
    {
        m_VertexBuffer = RingVertices(outerRadius, innerRadius, sides, rings);

        // Initialisation du VBO
        glGenBuffers(1, &_vbo);
        glBindBuffer(GL_ARRAY_BUFFER, _vbo);
        glBufferData(GL_ARRAY_BUFFER, m_VertexBuffer.size() * sizeof(Geometry::Vertex), m_VertexBuffer.data(), GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    static std::vector<unsigned int> generateRingIndices(unsigned int sides, unsigned int rings)
    {
        std::vector<unsigned int> indices;

        for (unsigned int ring = 0; ring < rings; ++ring)
        {
            for (unsigned int side = 0; side < sides; ++side)
            {
                unsigned int current = ring * sides + side;
                unsigned int next = current + sides;

                // First triangle
                indices.push_back(current);
                indices.push_back(next % (sides * rings));
                indices.push_back((current + 1) % sides + ring * sides);

                // Second triangle
                indices.push_back((current + 1) % sides + ring * sides);
                indices.push_back(next % (sides * rings));
                indices.push_back((next + 1) % sides + (ring + 1) * sides % (sides * rings));
            }
        }

        return indices;
    }

    void initMesh(unsigned int sides, unsigned int rings)
    {
        size_t lastMeshIndex = getLastMeshIndex();
        size_t newIndex = lastMeshIndex + m_VertexBuffer.size();
        auto indices = generateRingIndices(sides, rings);

        Geometry::Mesh mesh("ring", lastMeshIndex, newIndex, -1);
        updateLastMeshIndex(newIndex);

        const GLuint VERTEX_ATTR_POSITION = 1;
        const GLuint VERTEX_ATTR_NORMAL = 2;
        const GLuint VERTEX_ATTR_TEX = 3;

        glGenBuffers(1, &_ibo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ibo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

        glGenVertexArrays(1, &mesh.vao);
        glBindVertexArray(mesh.vao);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ibo);

        glBindBuffer(GL_ARRAY_BUFFER, _vbo);

        glEnableVertexAttribArray(VERTEX_ATTR_POSITION);
        glEnableVertexAttribArray(VERTEX_ATTR_NORMAL);
        glEnableVertexAttribArray(VERTEX_ATTR_TEX);

        glVertexAttribPointer(VERTEX_ATTR_POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(Geometry::Vertex), (GLvoid *)offsetof(Geometry::Vertex, m_Position));
        glVertexAttribPointer(VERTEX_ATTR_NORMAL, 3, GL_FLOAT, GL_FALSE, sizeof(Geometry::Vertex), (GLvoid *)offsetof(Geometry::Vertex, m_Normal));
        glVertexAttribPointer(VERTEX_ATTR_TEX, 2, GL_FLOAT, GL_FALSE, sizeof(Geometry::Vertex), (GLvoid *)offsetof(Geometry::Vertex, m_TexCoords));

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

        m_MeshBuffer.emplace_back(std::move(mesh));
    }

public:
    Ring(GLfloat outerRadius, GLfloat innerRadius, unsigned int sides, unsigned int rings)
    {
        build(outerRadius, innerRadius, sides, rings);
        initMesh(sides, rings);
    }

    // Fonction pour créer les vertices du tube (anneau)
    static std::vector<Geometry::Vertex> RingVertices(GLfloat outerRadius, GLfloat innerRadius, unsigned int sides, unsigned int rings)
    {
        std::vector<Geometry::Vertex> vertices;

        // Boucle sur les anneaux (le long du tube)
        for (unsigned int ring = 0; ring < rings; ++ring)
        {
            float theta = (float)ring / rings * 2.0f * M_PI; // Angle autour de l'axe Y
            float cosTheta = cosf(theta);
            float sinTheta = sinf(theta);

            // Boucle sur les côtés du tube
            for (unsigned int side = 0; side < sides; ++side)
            {
                float phi = (float)side / sides * 2.0f * M_PI; // Angle autour du tube (plan X-Z)
                float cosPhi = cosf(phi);
                float sinPhi = sinf(phi);

                // Position du vertex sur le tube de l'anneau
                glm::vec3 position = glm::vec3(
                    (outerRadius + innerRadius * cosPhi) * cosTheta,
                    innerRadius * sinPhi,
                    (outerRadius + innerRadius * cosPhi) * sinTheta);

                // Normalisation de la normale (pour l'éclairage)
                glm::vec3 normal = glm::normalize(glm::vec3(
                    cosPhi * cosTheta,
                    sinPhi,
                    cosPhi * sinTheta));

                // Coordonnées de texture
                glm::vec2 texCoords = glm::vec2((float)side / sides, (float)ring / rings);

                // Ajouter le vertex dans le vecteur
                vertices.push_back({position, normal, texCoords});
            }
        }

        return vertices;
    }
};
