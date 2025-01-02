#pragma once
#include "Geometry.hpp"
#include <cmath>

class Tore : public Geometry
{
private:
    GLfloat _radius;

    void build(GLfloat R, GLfloat r, int numSegments = 36, int numCirclePoints = 36)
    {
        for (int i = 0; i < numSegments; ++i)
        {
            GLfloat theta = (i / GLfloat(numSegments)) * 2.0f * M_PI; // Angle around the big circle

            for (int j = 0; j < numCirclePoints; ++j)
            {
                GLfloat phi = (j / GLfloat(numCirclePoints)) * 2.0f * M_PI; // Angle around the tube

                // Parametric equations of the torus
                GLfloat x = (R + r * cos(phi)) * cos(theta);
                GLfloat y = (R + r * cos(phi)) * sin(theta);
                GLfloat z = r * sin(phi);

                glm::vec3 position(x, y, z);

                // Normal vector (direction of the surface at the vertex)
                glm::vec3 normal = glm::normalize(position - glm::vec3(R, 0, 0));

                // Texture coordinates
                glm::vec2 texCoords(float(i) / numSegments, float(j) / numCirclePoints);

                Geometry::Vertex vertex = {position, normal, texCoords};
                m_VertexBuffer.push_back(vertex);

                // Generate indices for drawing triangles
                int nextI = (i + 1) % numSegments;
                int nextJ = (j + 1) % numCirclePoints;

                // Indices for two triangles making up a quadrilateral
                unsigned int idx0 = i * numCirclePoints + j;
                unsigned int idx1 = nextI * numCirclePoints + j;
                unsigned int idx2 = i * numCirclePoints + nextJ;
                unsigned int idx3 = nextI * numCirclePoints + nextJ;

                // First triangle
                m_IndexBuffer.push_back(idx0);
                m_IndexBuffer.push_back(idx2);
                m_IndexBuffer.push_back(idx1);

                // Second triangle
                m_IndexBuffer.push_back(idx1);
                m_IndexBuffer.push_back(idx2);
                m_IndexBuffer.push_back(idx3);
            }
        }

        // Init VBO
        glGenBuffers(1, &this->_vbo);
        glBindBuffer(GL_ARRAY_BUFFER, this->_vbo);
        glBufferData(GL_ARRAY_BUFFER, this->m_VertexBuffer.size() * sizeof(Geometry::Vertex), this->getVertexBuffer(), GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glGenBuffers(1, &this->_ibo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->_ibo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_IndexBuffer.size() * sizeof(unsigned int), m_IndexBuffer.data(), GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

    void initMesh()
    {
        size_t lastMeshIndex = getLastMeshIndex();
        size_t newIndex = lastMeshIndex + this->m_VertexBuffer.size();

        Geometry::Mesh mesh("tore", lastMeshIndex, newIndex, -1);
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
        glVertexAttribPointer(VERTEX_ATTR_TEX, 2, GL_FLOAT, GL_FALSE, sizeof(Geometry::Vertex), (GLvoid *)offsetof(Geometry::Vertex, m_TexCoords));
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        this->m_MeshBuffer.emplace_back(std::move(mesh));
    }

public:
    Tore(GLfloat R, GLfloat r, int numSegments = 36, int numCirclePoints = 36) : _radius(R)
    {
        build(R, r, numSegments, numCirclePoints);
        initMesh();
    }

    Tore(GLfloat R, GLfloat r, Geometry::Material material, int numSegments = 36, int numCirclePoints = 36) : _radius(R)
    {
        m_Materials.push_back(material);
        build(R, r, numSegments, numCirclePoints);
        initMesh();
    }

    void applyTwist(float twistAmount)
    {
        // Similar to Cube class, we can apply a twist on the vertices based on their Y-coordinate
        auto size = m_VertexBuffer.size();
        for (int i = 0; i < size; ++i)
        {
            float angle = twistAmount * (m_VertexBuffer[i].m_Position.y / (2 * _radius));
            float x = m_VertexBuffer[i].m_Position.x;
            float z = m_VertexBuffer[i].m_Position.z;

            // Apply twist (rotation around the Y-axis)
            m_VertexBuffer[i].m_Position.x = x * cos(angle) - z * sin(angle);
            m_VertexBuffer[i].m_Position.z = x * sin(angle) + z * cos(angle);
        }

        glBindBuffer(GL_ARRAY_BUFFER, this->_vbo);
        glBufferData(GL_ARRAY_BUFFER, this->m_VertexBuffer.size() * sizeof(Geometry::Vertex), this->getVertexBuffer(), GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
};
