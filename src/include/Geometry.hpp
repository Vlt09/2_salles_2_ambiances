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

        bool isTransform = true;
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
    size_t _lastMeshIndex = 0;

    void generateNormals(unsigned int meshIndex);

protected:
    std::vector<Mesh> m_MeshBuffer;
    std::vector<Vertex> m_VertexBuffer;
    std::vector<Material> m_Materials;
    glm::mat4 _modelMatrix = glm::mat4(1);
    std::vector<unsigned int> m_IndexBuffer;
    glimac::BBox3f m_BBox = glimac::BBox3f(glm::vec3(FLT_MAX), glm::vec3(-FLT_MAX));

    GLuint _vbo, _ibo = -1, _tex = -1;

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

    GLuint getIBO() const
    {
        return _ibo;
    }

    const glimac::BBox3f &getBBox()
    {
        return m_BBox;
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

    const std::vector<Material> &getMaterialList() const
    {
        return m_Materials;
    }

    size_t getLastMeshIndex() const
    {
        return _lastMeshIndex;
    }

    void updateLastMeshIndex(size_t index)
    {
        _lastMeshIndex = index;
    }

    GLuint getTex() const
    {
        return _tex;
    }

    const std::vector<Mesh> &getMeshVector() const
    {
        return m_MeshBuffer;
    }

    /**
     * @brief This function add shape which is represents by array
     * of vertices in the current Geometry object. In other word,
     * this function add one Mesh which is a simple shape (quad, sphere etc).
     */
    glimac::BBox3f addFromVertices(std::vector<Geometry::Vertex> vertices, int matIndex, glm::mat4 transformMatrix);

    /**
     * @brief This function adds material to material list
     */
    void addMaterial(const Material &material);

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

    void calculateBoundingBox();

    /**
     * @brief Calculate Bbox from a Mesh in world space
     */
    glimac::BBox3f bBoxFromMesh(size_t begin, size_t size, const glm::mat4 &modelMatrix);

    void translateModel(const glm::vec3 &translate);

    void translateMesh(const glm::vec3 &translate, unsigned int idX);

    void translateMeshWithIdentity(const glm::vec3 &translate, unsigned int idX);

    void scaleModel(const glm::vec3 &scale);

    void scaleMesh(const glm::vec3 &scale, unsigned int idX);

    void rotateModel(float angle, const glm::vec3 &axis);

    void rotateMesh(float angle, const glm::vec3 &axis, unsigned int idX);
};
