#pragma once
#include "Geometry.hpp"
#include "Cone.hpp"
#include "Room.hpp"

class Cube : public Geometry
{
private:
    Room::UniformVariable _uniformVariable;

    void build(GLfloat size)
    {
        m_VertexBuffer = CubeVertices(size);

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

        Geometry::Mesh mesh("cube", lastMeshIndex, newIndex, -1);
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
    Cube(GLfloat size)
    {
        build(size);
        initMesh();
    }

    void setUniformVariable(const Room::UniformVariable &uniformVariable)
    {
        _uniformVariable = uniformVariable;
    }

    Cube(GLfloat size, Geometry::Material material)
    {
        m_Materials.push_back(material);
        build(size); // Construction (voir le .cpp)
        initMesh();
    }

    static std::vector<Geometry::Vertex> CubeVertices(GLfloat size)
    {
        GLfloat halfSize = size / 2.f;

        // Define the vertices for a cube (6 faces, each with 2 triangles)
        std::vector<Geometry::Vertex> vertices = {
            // Front face
            {{-halfSize, -halfSize, halfSize}, {0.f, 0.f, 1.f}, {0.f, 0.f}},
            {{halfSize, -halfSize, halfSize}, {0.f, 0.f, 1.f}, {1.f, 0.f}},
            {{halfSize, halfSize, halfSize}, {0.f, 0.f, 1.f}, {1.f, 1.f}},

            {{-halfSize, -halfSize, halfSize}, {0.f, 0.f, 1.f}, {0.f, 0.f}},
            {{halfSize, halfSize, halfSize}, {0.f, 0.f, 1.f}, {1.f, 1.f}},
            {{-halfSize, halfSize, halfSize}, {0.f, 0.f, 1.f}, {0.f, 1.f}},

            // Back face
            {{-halfSize, -halfSize, -halfSize}, {0.f, 0.f, -1.f}, {0.f, 0.f}},
            {{-halfSize, halfSize, -halfSize}, {0.f, 0.f, -1.f}, {1.f, 0.f}},
            {{halfSize, halfSize, -halfSize}, {0.f, 0.f, -1.f}, {1.f, 1.f}},

            {{-halfSize, -halfSize, -halfSize}, {0.f, 0.f, -1.f}, {0.f, 0.f}},
            {{halfSize, halfSize, -halfSize}, {0.f, 0.f, -1.f}, {1.f, 1.f}},
            {{halfSize, -halfSize, -halfSize}, {0.f, 0.f, -1.f}, {0.f, 1.f}},

            // Left face
            {{-halfSize, -halfSize, -halfSize}, {-1.f, 0.f, 0.f}, {0.f, 0.f}},
            {{-halfSize, -halfSize, halfSize}, {-1.f, 0.f, 0.f}, {1.f, 0.f}},
            {{-halfSize, halfSize, halfSize}, {-1.f, 0.f, 0.f}, {1.f, 1.f}},

            {{-halfSize, -halfSize, -halfSize}, {-1.f, 0.f, 0.f}, {0.f, 0.f}},
            {{-halfSize, halfSize, halfSize}, {-1.f, 0.f, 0.f}, {1.f, 1.f}},
            {{-halfSize, halfSize, -halfSize}, {-1.f, 0.f, 0.f}, {0.f, 1.f}},

            // Right face
            {{halfSize, -halfSize, -halfSize}, {1.f, 0.f, 0.f}, {0.f, 0.f}},
            {{halfSize, halfSize, -halfSize}, {1.f, 0.f, 0.f}, {1.f, 0.f}},
            {{halfSize, halfSize, halfSize}, {1.f, 0.f, 0.f}, {1.f, 1.f}},

            {{halfSize, -halfSize, -halfSize}, {1.f, 0.f, 0.f}, {0.f, 0.f}},
            {{halfSize, halfSize, halfSize}, {1.f, 0.f, 0.f}, {1.f, 1.f}},
            {{halfSize, -halfSize, halfSize}, {1.f, 0.f, 0.f}, {0.f, 1.f}},

            // Top face
            {{-halfSize, halfSize, -halfSize}, {0.f, 1.f, 0.f}, {0.f, 0.f}},
            {{halfSize, halfSize, -halfSize}, {0.f, 1.f, 0.f}, {1.f, 0.f}},
            {{halfSize, halfSize, halfSize}, {0.f, 1.f, 0.f}, {1.f, 1.f}},

            {{-halfSize, halfSize, -halfSize}, {0.f, 1.f, 0.f}, {0.f, 0.f}},
            {{halfSize, halfSize, halfSize}, {0.f, 1.f, 0.f}, {1.f, 1.f}},
            {{-halfSize, halfSize, halfSize}, {0.f, 1.f, 0.f}, {0.f, 1.f}},

            // Bottom face
            {{-halfSize, -halfSize, -halfSize}, {0.f, -1.f, 0.f}, {0.f, 0.f}},
            {{-halfSize, -halfSize, halfSize}, {0.f, -1.f, 0.f}, {1.f, 0.f}},
            {{halfSize, -halfSize, halfSize}, {0.f, -1.f, 0.f}, {1.f, 1.f}},

            {{-halfSize, -halfSize, -halfSize}, {0.f, -1.f, 0.f}, {0.f, 0.f}},
            {{halfSize, -halfSize, halfSize}, {0.f, -1.f, 0.f}, {1.f, 1.f}},
            {{halfSize, -halfSize, -halfSize}, {0.f, -1.f, 0.f}, {0.f, 1.f}}};

        return vertices;
    }

    void applyTwist(float twistAmount)
    {
        auto size = m_VertexBuffer.size();
        for (int i = 0; i < size; i++)
        {
            // Calculate the angle based on the Y-coordinate of the vertex
            float angle = twistAmount * (m_VertexBuffer[i].m_Position.y + 1.0f); // Normalize Y to range [-1, 1]

            // Apply the rotation around the Y-axis
            float x = m_VertexBuffer[i].m_Position.x;
            float z = m_VertexBuffer[i].m_Position.z;

            // Rotate around Y-axis (cos(angle) for x and sin(angle) for z)
            m_VertexBuffer[i].m_Position.x = x * cos(angle) - z * sin(angle);
            m_VertexBuffer[i].m_Position.z = x * sin(angle) + z * cos(angle);
        }

        // Rebuild the VBO after modifying the vertices
        glBindBuffer(GL_ARRAY_BUFFER, this->_vbo);
        glBufferData(GL_ARRAY_BUFFER, this->m_VertexBuffer.size() * sizeof(Geometry::Vertex), this->getVertexBuffer(), GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
};
