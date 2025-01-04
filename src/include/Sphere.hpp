#pragma once
#include <vector>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Geometry.hpp"
#include <glimac/common.hpp>
#include <glimac/getTime.hpp>

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
        Geometry::Mesh mesh("sphere", lastMeshIndex, newIndex, idMat);
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
    Sphere()
    {
    }

    // Constructeur: alloue le tableau de données et construit les attributs des vertex
    Sphere(GLfloat radius, GLsizei discLat, GLsizei discLong) : m_nVertexCount(0), _discLat(discLat), _discLong(discLong), _radius(radius)
    {
        build(radius, discLat, discLong); // Construction (voir le .cpp)
        initMesh();
        // translateModel(0.f, 0.f, -5.f);
    }

    Sphere(GLfloat radius, GLsizei discLat, GLsizei discLong, Geometry::Material material) : m_nVertexCount(0),
                                                                                             _discLat(discLat),
                                                                                             _discLong(discLong),
                                                                                             _radius(radius)
    {
        m_Materials.push_back(material);
        build(radius, discLat, discLong); // Construction (voir le .cpp)
        initMesh();
        // translateModel(0.f, 0.f, -5.f);
    }

    void initSphere(GLfloat radius, GLsizei discLat, GLsizei discLong, Geometry::Material sphereMat)
    {
        m_Materials.push_back(sphereMat);
        build(radius, discLat, discLong);
        initMesh();
        _discLat = discLat;
        _discLong = discLong;
        _radius = radius;
    }

    /**
     * @brief Computes the position of a point on the surface of a sphere, uniformly distributed using the Fibonacci spiral method.
     *
     * This method ensures an even distribution of points on the sphere's surface, avoiding clustering at the poles.
     *
     * @param index The index of the current point (must be in the range [0, total - 1]).
     * @param total The total number of points to distribute on the sphere.
     *
     * @return glm::vec3 The 3D position of the point on the sphere's surface.
     *
     * @details
     * - The Fibonacci spiral uses a "golden angle" to distribute points:
     *   - `phi = π * (3 - sqrt(5))`
     * - The `y` coordinate is spaced evenly between -1 (South Pole) and 1 (North Pole), ensuring uniform vertical distribution.
     * - The horizontal radius `r` is calculated as:
     *   - `r = sqrt(1 - y^2)`
     * - The horizontal angle `theta` determines the spiral rotation:
     *   - `theta = phi * index`
     * - The final position is computed in Cartesian coordinates as:
     *   - `x = r * cos(theta)`
     *   - `z = r * sin(theta)`
     */
    glm::vec3 getPointOnSphere(int index, int total)
    {
        float y = 1.f - (index / float(total - 1)) * 2.f; // Uniform y distribution
        float r = sqrt(1.f - y * y);
        float theta = _phi * index;

        float x = r * cos(theta);
        float z = r * sin(theta);

        return glm::vec3(x, y, z) * _radius; // Scale by radius
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
    GLsizei m_nVertexCount, _discLat, _discLong; // Nombre de sommets
    GLfloat _radius;
    float _phi = glm::pi<float>() * (3.f - sqrt(5.f)); // Golden angle
};