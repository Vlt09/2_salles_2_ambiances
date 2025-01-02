#pragma once
#include "Geometry.hpp"
#include <cmath>

class Tore : public Geometry
{
private:
    GLfloat _radius;

    void build(GLfloat R, GLfloat r, int numSegments = 36, int numCirclePoints = 36)
    {
        // Generate the vertices for the Tore
        m_VertexBuffer = generateToreVertices(R, r, numSegments, numCirclePoints);

        // Init VBO
        glGenBuffers(1, &this->_vbo);
        glBindBuffer(GL_ARRAY_BUFFER, this->_vbo);
        glBufferData(GL_ARRAY_BUFFER, this->m_VertexBuffer.size() * sizeof(Geometry::Vertex), this->getVertexBuffer(), GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
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

    // Generate vertices for the Tore based on parametric equations
    static std::vector<Geometry::Vertex> generateToreVertices(GLfloat R, GLfloat r, int numSegments, int numCirclePoints)
    {
        std::vector<Geometry::Vertex> vertices;

        // Parametric equations for the Tore
        for (int i = 0; i < numSegments; ++i)
        {
            float u = (float)i / numSegments * 2.0f * M_PI;
            for (int j = 0; j < numCirclePoints; ++j)
            {
                float v = (float)j / numCirclePoints * 2.0f * M_PI;

                glm::vec3 position;
                position.x = (R + r * cos(v)) * cos(u);
                position.y = (R + r * cos(v)) * sin(u);
                position.z = r * sin(v);

                glm::vec3 tangentU, tangentV, normal;
                tangentU = glm::vec3(-sin(u), cos(u), 0.0f);
                tangentV = glm::vec3(cos(v) * cos(u), cos(v) * sin(u), sin(v));
                normal = glm::normalize(glm::cross(tangentU, tangentV));

                glm::vec2 texCoord = glm::vec2((float)i / numSegments, (float)j / numCirclePoints);

                vertices.push_back({position, normal, texCoord});
            }
        }

        return vertices;
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
