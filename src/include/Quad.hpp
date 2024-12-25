#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Geometry.hpp"

class Quad : public Geometry
{

    void build(GLfloat width, GLfloat height)
    {
        m_VertexBuffer = QuadVertices(height, width);
    }

    void initMesh()
    {
        size_t lastMeshIndex = getLastMeshIndex();
        size_t newIndex = lastMeshIndex + this->m_VertexBuffer.size();

        Geometry::Mesh mesh("quad", lastMeshIndex, newIndex, -1);
        updateLastMeshIndex(newIndex);

        const GLuint VERTEX_ATTR_POSITION = 1;
        const GLuint VERTEX_ATTR_NORMAL = 2;
        const GLuint VERTEX_ATTR_TEX = 3;

        glGenBuffers(1, &mesh.vbo);
        glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
        glBufferData(GL_ARRAY_BUFFER, this->m_VertexBuffer.size() * sizeof(Geometry::Vertex), this->getVertexBuffer(), GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glGenVertexArrays(1, &mesh.vao);
        glBindVertexArray(mesh.vao);
        glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
        glEnableVertexAttribArray(VERTEX_ATTR_POSITION);
        glEnableVertexAttribArray(VERTEX_ATTR_NORMAL);
        glVertexAttribPointer(VERTEX_ATTR_POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(Geometry::Vertex), offsetof(Geometry::Vertex, m_Position));
        glVertexAttribPointer(VERTEX_ATTR_NORMAL, 3, GL_FLOAT, GL_FALSE, sizeof(Geometry::Vertex), (GLvoid *)offsetof(Geometry::Vertex, m_Normal));
        glVertexAttribPointer(VERTEX_ATTR_TEX, 3, GL_FLOAT, GL_FALSE, sizeof(Geometry::Vertex), (GLvoid *)offsetof(Geometry::Vertex, m_TexCoords));
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        this->m_MeshBuffer.emplace_back(std::move(mesh));
    }

private:
    GLsizei m_nVertexCount;

public:
    Quad(GLfloat width, GLfloat height)
    {
        build(width, height);
        initMesh();
        // translateModel(0.f, 0.f, -5.f);
    }

    /**
     * @brief Defines vertices for a Quad and return vector that contains it
     */
    static std::vector<Geometry::Vertex> QuadVertices(GLfloat height, GLfloat width)
    {
        GLfloat halfWidth = width / 2.f;
        GLfloat halfHeight = height / 2.f;

        // Define the vertices for a quad (two triangles forming a rectangle)
        std::vector<Geometry::Vertex> vertices = {
            {{-halfWidth, 0.f, -halfHeight}, {0.f, 1.f, 0.f}, {0.f, 0.f}}, // Bottom-left
            {{halfWidth, 0.f, -halfHeight}, {0.f, 1.f, 0.f}, {1.f, 0.f}},  // Bottom-right
            {{halfWidth, 0.f, halfHeight}, {0.f, 1.f, 0.f}, {1.f, 1.f}},   // Top-right

            {{-halfWidth, 0.f, -halfHeight}, {0.f, 1.f, 0.f}, {0.f, 0.f}}, // Bottom-left
            {{halfWidth, 0.f, halfHeight}, {0.f, 1.f, 0.f}, {1.f, 1.f}},   // Top-right
            {{-halfWidth, 0.f, halfHeight}, {0.f, 1.f, 0.f}, {0.f, 1.f}}   // Top-left
        };

        return vertices;
    }
};
