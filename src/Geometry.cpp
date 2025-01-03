#include "include/Geometry.hpp"
#include "../glimac/src/tiny_obj_loader.h"
#include <iostream>
#include <algorithm>

void Geometry::generateNormals(unsigned int meshIndex)
{
    auto indexOffset = m_MeshBuffer[meshIndex].m_nIndexOffset;
    for (auto j = 0u; j < m_MeshBuffer[meshIndex].m_nIndexCount; j += 3)
    {
        auto i1 = m_IndexBuffer[indexOffset + j];
        auto i2 = m_IndexBuffer[indexOffset + j + 1];
        auto i3 = m_IndexBuffer[indexOffset + j + 2];

        auto n = glm::cross(glm::normalize(m_VertexBuffer[i2].m_Position - m_VertexBuffer[i1].m_Position),
                            glm::normalize(m_VertexBuffer[i3].m_Position - m_VertexBuffer[i1].m_Position));

        m_VertexBuffer[i1].m_Normal = n;
        m_VertexBuffer[i2].m_Normal = n;
        m_VertexBuffer[i3].m_Normal = n;
    }
}

bool Geometry::loadOBJ(const glimac::FilePath &filepath, const glimac::FilePath &mtlBasePath, bool loadTextures)
{
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;

    std::clog << "Load OBJ " << filepath << std::endl;
    std::string objErr = tinyobj::LoadObj(shapes, materials,
                                          filepath.c_str(), mtlBasePath.c_str());

    std::clog << "done." << std::endl;

    if (!objErr.empty())
    {
        std::cerr << objErr << std::endl;
        return false;
    }

    std::clog << "Load materials" << std::endl;
    m_Materials.reserve(m_Materials.size() + materials.size());
    for (auto &material : materials)
    {
        m_Materials.emplace_back();
        auto &m = m_Materials.back();

        m.m_Ka = glm::vec3(material.ambient[0], material.ambient[1], material.ambient[2]);
        m.m_Kd = glm::vec3(material.diffuse[0], material.diffuse[1], material.diffuse[2]);
        m.m_Ks = glm::vec3(material.specular[0], material.specular[1], material.specular[2]);
        m.m_Tr = glm::vec3(material.transmittance[0], material.transmittance[1], material.transmittance[2]);
        m.m_Le = glm::vec3(material.emission[0], material.emission[1], material.emission[2]);
        m.m_Shininess = material.shininess;
        m.m_RefractionIndex = material.ior;
        m.m_Dissolve = material.dissolve;

        if (loadTextures)
        {
            if (!material.ambient_texname.empty())
            {
                // std::replace(material.ambient_texname.begin(), material.ambient_texname.end(), '\\', '/');
                glimac::FilePath texturePath = mtlBasePath + material.ambient_texname;
                std::clog << "load " << texturePath << std::endl;
                m.m_pKaMap = glimac::ImageManager::loadImage(texturePath);
            }

            if (!material.diffuse_texname.empty())
            {
                // std::replace(material.diffuse_texname.begin(), material.diffuse_texname.end(), '\\', '/');
                glimac::FilePath texturePath = mtlBasePath + material.diffuse_texname;
                std::clog << "load " << texturePath << std::endl;
                m.m_pKdMap = glimac::ImageManager::loadImage(texturePath);
            }

            if (!material.specular_texname.empty())
            {
                // std::replace(material.specular_texname.begin(), material.specular_texname.end(), '\\', '/');
                glimac::FilePath texturePath = mtlBasePath + material.specular_texname;
                std::clog << "load " << texturePath << std::endl;
                m.m_pKsMap = glimac::ImageManager::loadImage(texturePath);
            }

            if (!material.normal_texname.empty())
            {
                // std::replace(material.normal_texname.begin(), material.normal_texname.end(), '\\', '/');
                glimac::FilePath texturePath = mtlBasePath + material.normal_texname;
                std::clog << "load " << texturePath << std::endl;
                m.m_pNormalMap = glimac::ImageManager::loadImage(texturePath);
            }
        }
    }
    std::clog << "done." << std::endl;

    auto globalVertexOffset = m_VertexBuffer.size();
    auto globalIndexOffset = m_IndexBuffer.size();

    auto nbVertex = 0u;
    auto nbIndex = 0u;
    for (const auto &shape : shapes)
    {
        nbVertex += shape.mesh.positions.size();
        nbIndex += shape.mesh.indices.size();
    }

    std::clog << "Number of meshes: " << shapes.size() << std::endl;
    std::clog << "Number of vertices: " << nbVertex << std::endl;
    std::clog << "Number of triangles: " << (nbIndex) / 3 << std::endl;

    m_BBox = glimac::BBox3f(glm::vec3(shapes[0].mesh.positions[0], shapes[0].mesh.positions[1], shapes[0].mesh.positions[2]));

    m_VertexBuffer.resize(m_VertexBuffer.size() + nbVertex);
    m_IndexBuffer.resize(m_IndexBuffer.size() + nbIndex);

    auto pVertex = (Vertex *)m_VertexBuffer.data() + globalVertexOffset;
    auto pIndex = (unsigned int *)m_IndexBuffer.data() + globalIndexOffset;

    m_MeshBuffer.reserve(m_MeshBuffer.size() + shapes.size());

    auto vertexOffset = globalVertexOffset;
    auto indexOffset = globalIndexOffset;
    for (size_t i = 0; i < shapes.size(); i++)
    {
        auto pVertexTmp = pVertex;
        for (auto j = 0u; j < shapes[i].mesh.positions.size(); j += 3)
        {
            pVertexTmp->m_Position.x = shapes[i].mesh.positions[j];
            pVertexTmp->m_Position.y = shapes[i].mesh.positions[j + 1];
            pVertexTmp->m_Position.z = shapes[i].mesh.positions[j + 2];

            m_BBox.grow(pVertexTmp->m_Position);

            ++pVertexTmp;
        }
        pVertexTmp = pVertex;
        if (shapes[i].mesh.normals.size())
        {
            for (auto j = 0u; j < shapes[i].mesh.normals.size(); j += 3)
            {
                pVertexTmp->m_Normal.x = shapes[i].mesh.normals[j];
                pVertexTmp->m_Normal.y = shapes[i].mesh.normals[j + 1];
                pVertexTmp->m_Normal.z = shapes[i].mesh.normals[j + 2];
                ++pVertexTmp;
            }
        }
        else
        {
            // should create normals from triangles...
        }
        pVertexTmp = pVertex;
        if (shapes[i].mesh.texcoords.size())
        {
            for (auto j = 0u; j < shapes[i].mesh.texcoords.size(); j += 2)
            {
                pVertexTmp->m_TexCoords.x = shapes[i].mesh.texcoords[j];
                pVertexTmp->m_TexCoords.y = shapes[i].mesh.texcoords[j + 1];
                ++pVertexTmp;
            }
        }
        else
        {
            for (auto j = 0u; j < shapes[i].mesh.texcoords.size(); j += 2)
            {
                pVertexTmp->m_TexCoords.x = 0.f;
                pVertexTmp->m_TexCoords.x = 0.f;
                ++pVertexTmp;
            }
        }

        for (auto j = 0u; j < shapes[i].mesh.indices.size(); ++j)
        {
            pIndex[j] = vertexOffset + shapes[i].mesh.indices[j];
        }

        int materialIndex = -1;
        if (!shapes[i].mesh.material_ids.empty())
        {
            materialIndex = shapes[i].mesh.material_ids[0];
        }

        m_MeshBuffer.emplace_back(shapes[i].name, indexOffset, shapes[i].mesh.indices.size(), materialIndex);

        if (shapes[i].mesh.normals.size() == 0u)
        {
            generateNormals(m_MeshBuffer.size() - 1);
        }

        pVertex += shapes[i].mesh.positions.size();
        vertexOffset += shapes[i].mesh.positions.size();
        pIndex += shapes[i].mesh.indices.size();
        indexOffset += shapes[i].mesh.indices.size();
    }

    return true;
}

Geometry::Mesh &Geometry::addFromVertices(std::vector<Geometry::Vertex> vertices, int matIndex, glm::mat4 transformMatrix)
{
    m_VertexBuffer.insert(m_VertexBuffer.end(), vertices.begin(), vertices.end());

    size_t newIndex = lastMeshIndex + vertices.size();
    Geometry::Mesh mesh("shape", lastMeshIndex, vertices.size(), matIndex);
    mesh._transform = transformMatrix;
    mesh.isTransform = true;

    updateLastMeshIndex(newIndex);
    m_MeshBuffer.push_back(std::move(mesh));

    m_BBox = glimac::merge(m_BBox, bBoxFromMesh(newIndex, vertices.size(), mesh._transform)); // calc BBox in world space
    std::cout << "BBox = " << m_BBox << std::endl;

    return m_MeshBuffer.back();
}

void Geometry::addMaterial(const Material &material)
{
    m_Materials.emplace_back(material);
}

void Geometry::initMeshData()
{
    const GLuint VERTEX_ATTR_POSITION = 1;
    const GLuint VERTEX_ATTR_NORMAL = 2;
    const GLuint VERTEX_ATTR_TEX = 3;

    // Init VBO
    glGenBuffers(1, &this->_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, this->_vbo);
    glBufferData(GL_ARRAY_BUFFER, this->m_VertexBuffer.size() * sizeof(Geometry::Vertex), this->getVertexBuffer(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    for (auto &mesh : m_MeshBuffer)
    {
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
    }
}

void Geometry::initTexture(const std::string &path)
{
    auto tex_ptr = glimac::loadImage(path);
    if (tex_ptr == NULL)
    {
        std::cout << "Failed to load texture" << std::endl;
        return;
    }
    glGenTextures(1, &_tex);
    glBindTexture(GL_TEXTURE_2D, _tex);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex_ptr->getWidth(), tex_ptr->getHeight(), 0, GL_RGBA, GL_FLOAT, tex_ptr->getPixels());

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindTexture(GL_TEXTURE_2D, 0);
}

void Geometry::translateModel(const glm::vec3 &translate)
{
    m_MeshBuffer[0]._transform = glm::translate(m_MeshBuffer[0]._transform, translate);
}

void Geometry::translateMesh(const glm::vec3 &translate, unsigned int idX)
{
    m_MeshBuffer[idX]._transform = glm::translate(m_MeshBuffer[0]._transform, translate);
}

void Geometry::scaleModel(const glm::vec3 &scale)
{
    m_MeshBuffer[0]._transform = glm::scale(m_MeshBuffer[0]._transform, scale);
}

void Geometry::scaleMesh(const glm::vec3 &scale, unsigned int idX)
{
    m_MeshBuffer[idX]._transform = glm::scale(m_MeshBuffer[idX]._transform, scale);
}

void Geometry::rotateModel(float angle, const glm::vec3 &axis)
{
    m_MeshBuffer[0]._transform = glm::rotate(m_MeshBuffer[0]._transform, angle, axis);
}

void Geometry::rotateMesh(float angle, const glm::vec3 &axis, unsigned int idX)
{
    m_MeshBuffer[idX]._transform = glm::rotate(m_MeshBuffer[idX]._transform, angle, axis);
}

void Geometry::calculateBoundingBox()
{
    if (m_VertexBuffer.empty())
        return;

    glm::vec3 min(FLT_MAX);
    glm::vec3 max(-FLT_MAX);

    for (const auto &vertex : m_VertexBuffer)
    {
        min = glm::min(min, vertex.m_Position);
        max = glm::max(max, vertex.m_Position);
    }

    auto minWS = glm::vec3(m_MeshBuffer[0]._transform * glm::vec4(min, 1.0));
    auto maxWS = glm::vec3(m_MeshBuffer[0]._transform * glm::vec4(max, 1.0));

    m_BBox = glimac::BBox3f(minWS, maxWS);
}

glimac::BBox3f Geometry::bBoxFromMesh(size_t begin, size_t size, const glm::mat4 &modelMatrix)
{
    glm::vec3 min(FLT_MAX);
    glm::vec3 max(-FLT_MAX);

    // std::cout << "transfo mat = " << modelMatrix << std::endl;
    size = begin + size;
    for (size_t i = begin; i < size; i++)
    {
        auto posWS = glm::vec3(modelMatrix * glm::vec4(m_VertexBuffer[i].m_Position, 1.0));
        min = glm::min(min, posWS);
        max = glm::max(max, posWS);
    }

    return glimac::BBox3f(min, max);
}
