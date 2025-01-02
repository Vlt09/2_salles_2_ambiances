#include "include/Cone.hpp"

#include <cmath>
#include <vector>
#include <iostream>
#include <glimac/common.hpp>

void Cone::build(GLfloat height, GLfloat r, GLsizei discLat, GLsizei discHeight)
{
    auto data = Cone::ConeVertices(height, r, discLat, discHeight);

    GLuint idx = 0;
    // Construit les vertex finaux en regroupant les données en triangles:
    // Pour une longitude donnée, les deux triangles formant une face sont de la forme:
    // (i, i + 1, i + discLat + 1), (i, i + discLat + 1, i + discLat)
    // avec i sur la bande correspondant à la longitude
    for (GLsizei j = 0; j < discHeight; ++j)
    {
        GLsizei offset = j * discLat;
        for (GLsizei i = 0; i < discLat; ++i)
        {
            m_VertexBuffer.push_back(data[offset + i]);
            m_VertexBuffer.push_back(data[offset + (i + 1) % discLat]);
            m_VertexBuffer.push_back(data[offset + discLat + (i + 1) % discLat]);
            m_VertexBuffer.push_back(data[offset + i]);
            m_VertexBuffer.push_back(data[offset + discLat + (i + 1) % discLat]);
            m_VertexBuffer.push_back(data[offset + i + discLat]);
        }
    }
}

void Cone::initMesh()
{
    size_t lastMeshIndex = getLastMeshIndex();
    size_t newIndex = lastMeshIndex + this->m_VertexBuffer.size();
    Geometry::Mesh mesh("cone", lastMeshIndex, newIndex, -1);
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
