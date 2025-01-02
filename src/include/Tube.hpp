#pragma once
#include "Geometry.hpp"
#include "Room.hpp"

class WeirdTube : public Geometry
{
private:
    void build(GLfloat radius, GLfloat height, unsigned int slices, unsigned int stacks)
    {
        m_VertexBuffer = TubeVertices(radius, height, slices, stacks);

        glGenBuffers(1, &this->_vbo);
        glBindBuffer(GL_ARRAY_BUFFER, this->_vbo);
        glBufferData(GL_ARRAY_BUFFER, this->m_VertexBuffer.size() * sizeof(Geometry::Vertex), this->getVertexBuffer(), GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    void initMesh()
    {
        size_t lastMeshIndex = getLastMeshIndex();
        size_t newIndex = lastMeshIndex + this->m_VertexBuffer.size();

        Geometry::Mesh mesh("tube", lastMeshIndex, newIndex, -1);
        updateLastMeshIndex(newIndex);

        const GLuint VERTEX_ATTR_POSITION = 1;
        const GLuint VERTEX_ATTR_NORMAL = 2;
        const GLuint VERTEX_ATTR_TEX = 3;

        // Vao creation and link to vbo
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
    WeirdTube(GLfloat radius, GLfloat height, unsigned int slices, unsigned int stacks)
    {
        build(radius, height, slices, stacks);
        initMesh();
    }

    WeirdTube(GLfloat radius, GLfloat height, unsigned int slices, unsigned int stacks, Geometry::Material material)
    {
        m_Materials.push_back(material);
        build(radius, height, slices, stacks); // Construction (voir le .cpp)
        initMesh();
    }

    static std::vector<Geometry::Vertex> TubeVertices(GLfloat radius, GLfloat height, unsigned int slices, unsigned int stacks)
    {
        std::vector<Geometry::Vertex> vertices;

        GLfloat angleStep = 2 * glm::pi<GLfloat>() / slices;
        GLfloat stackHeight = height / stacks;

        for (unsigned int i = 0; i <= slices; i++)
        {
            GLfloat angle = i * angleStep;
            GLfloat x = radius * cos(angle);
            GLfloat z = radius * sin(angle);

            vertices.push_back({{x, height / 2.f, z}, {0.f, 1.f, 0.f}, {i / (GLfloat)slices, 1.f}});

            vertices.push_back({{x, -height / 2.f, z}, {0.f, -1.f, 0.f}, {i / (GLfloat)slices, 0.f}});
        }

        for (unsigned int i = 0; i < slices; i++)
        {
            GLfloat angle = i * angleStep;
            GLfloat nextAngle = (i + 1) % slices * angleStep;

            for (unsigned int j = 0; j <= stacks; j++)
            {
                GLfloat y = -height / 2.f + j * stackHeight;

                GLfloat x1 = radius * cos(angle);
                GLfloat z1 = radius * sin(angle);

                GLfloat x2 = radius * cos(nextAngle);
                GLfloat z2 = radius * sin(nextAngle);

                GLfloat texCoordX1 = (GLfloat)i / slices;
                GLfloat texCoordX2 = (GLfloat)(i + 1) / slices;
                GLfloat texCoordY = (GLfloat)j / stacks;

                vertices.push_back({{x1, y, z1}, {cos(angle), 0.f, sin(angle)}, {texCoordX1, texCoordY}});
                vertices.push_back({{x2, y, z2}, {cos(nextAngle), 0.f, sin(nextAngle)}, {texCoordX2, texCoordY}});
            }
        }

        return vertices;
    }
};
