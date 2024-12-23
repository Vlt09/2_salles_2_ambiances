#pragma once

#include <GLFW/glfw3.h>
#include <glad/glad.h>
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
    std::vector<Mesh> m_MeshBuffer;
    std::vector<Material> m_Materials;
    glimac::BBox3f m_BBox;

    void generateNormals(unsigned int meshIndex);

protected:
    std::vector<Vertex> m_VertexBuffer;

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
};
