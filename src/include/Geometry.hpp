#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <vector>
#include <string>
#include <glimac/Image.hpp>
#include <glimac/FilePath.hpp>
#include <glimac/BBox.hpp>

class Geometry
{
public:
    struct Vertex
    {
        glm::vec3 m_Position;
        glm::vec3 m_Normal;
        glm::vec2 m_TexCoords;
    };

    struct Mesh
    {
        std::string m_sName;
        unsigned int m_nIndexOffset; // Offset in the index buffer
        unsigned int m_nIndexCount;  // Number of indices
        int m_nMaterialIndex;        // -1 if no material assigned

        GLuint vbo, vao;

        bool isTransform = false;
        glm::mat4 _transform = glm::mat4(1);

        Mesh(std::string name, unsigned int indexOffset, unsigned int indexCount, int materialIndex) : m_sName(move(name)), m_nIndexOffset(indexOffset), m_nIndexCount(indexCount), m_nMaterialIndex(materialIndex)
        {
        }
    };

    struct Material
    {
        glm::vec3 m_Ka;
        glm::vec3 m_Kd;
        glm::vec3 m_Ks;
        glm::vec3 m_Tr;
        glm::vec3 m_Le;
        float m_Shininess;
        float m_RefractionIndex;
        float m_Dissolve;
        const glimac::Image *m_pKaMap;
        const glimac::Image *m_pKdMap;
        const glimac::Image *m_pKsMap;
        const glimac::Image *m_pNormalMap;
    };

private:
    std::vector<unsigned int> m_IndexBuffer;
    std::vector<Material> m_Materials;
    glimac::BBox3f m_BBox;

    size_t lastMeshIndex = 0;

    void generateNormals(unsigned int meshIndex);

protected:
    std::vector<Mesh> m_MeshBuffer;
    std::vector<Vertex> m_VertexBuffer;
    glm::mat4 _modelMatrix = glm::mat4(1);
    GLuint _vbo, _tex;

public:
    const Vertex *getVertexBuffer() const
    {
        return m_VertexBuffer.data();
    }

    size_t getVertexCount() const
    {
        return m_VertexBuffer.size();
    }

    const unsigned int *getIndexBuffer() const
    {
        return m_IndexBuffer.data();
    }

    size_t getIndexCount() const
    {
        return m_IndexBuffer.size();
    }

    const Mesh *getMeshBuffer() const
    {
        return m_MeshBuffer.data();
    }

    size_t getMeshCount() const
    {
        return m_MeshBuffer.size();
    }

    bool loadOBJ(const glimac::FilePath &filepath, const glimac::FilePath &mtlBasePath, bool loadTextures = true);

    const glimac::BBox3f &getBoundingBox() const
    {
        return m_BBox;
    }

    const glm::mat4 &getModelMatrix() const
    {
        return _modelMatrix;
    }

    size_t getLastMeshIndex() const
    {
        return lastMeshIndex;
    }

    void updateLastMeshIndex(size_t index)
    {
        lastMeshIndex = index;
    }

    GLuint getTex() const
    {
        return _tex;
    }

    /**
     * @brief This function add shape which is represents by array
     * of vertices in the current Geometry object. In other word,
     * this function add one Mesh which is a simple shape (quad, sphere etc).
     */
    Geometry::Mesh &addFromVertices(std::vector<Geometry::Vertex> vertices);

    /**
     * @brief This function initialize VBO's object and vao of all Meshes in this
     * object.
     */
    void initMeshData();

    /**
     * @brief Initializes a 2D texture from an image file.
     *
     * This function loads an image from the specified file path and creates an OpenGL texture.
     * It binds the texture and sets its parameters for minification and magnification filters.
     *
     * @param path A constant reference to a string that specifies the file path of the image to load.
     */
    void initTexture(const std::string &path);

    void translateModel(float sx = 0.f, float sy = 0.f, float sz = 0.f);

    void scaleModel(float sx = 1.f, float sy = 1.f, float sz = 1.f);

    void rotateModel(float angle, glm::vec3 axis);
};