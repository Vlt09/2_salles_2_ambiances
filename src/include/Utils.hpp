#pragma once
#include "Geometry.hpp"
#include <random>
#include <vector>

namespace Utils
{

    struct SpotLightUniformVarLoc
    {
        GLuint position;
        GLuint direction;
        GLuint lightIntensity;
        GLuint lightPos;
        GLuint m_Kd;
        GLuint m_Ks;
        GLuint m_Ka;
        GLuint cutoff;
        GLuint exponent;
    };

    struct SpotLight
    {
        Sphere _spot;

        glm::vec3 position = glm::vec3(0., 0., 0.);
        glm::vec3 direction = glm::vec3(0., -1.f, 0.);
        glm::vec3 intensity;

        float cutoff, exponent;
        float amplitude = 5.f;
        float frequency = 0.2f;
        float speed = 3.f;

        SpotLight()
        {
        }

        SpotLight(Sphere spot, glm::vec3 pos, glm::vec3 dir, glm::vec3 inten, float cut, float exp)
            : _spot(spot), position(pos), direction(dir), intensity(inten), cutoff(cut), exponent(exp)
        {
        }

        SpotLight(Sphere spot, glm::vec3 pos, glm::vec3 dir, glm::vec3 inten, float cut, float exp, float freq, float ampl)
            : _spot(spot), position(pos), direction(dir), intensity(inten), cutoff(cut), exponent(exp), amplitude(ampl), frequency(freq)
        {
        }
    };

    inline float linearMapping(float value, float minDomain, float maxDomain, float minCodomain, float maxCodomain)
    {
        return (value - minDomain) * (maxCodomain - minCodomain) / (maxDomain - minDomain) + minCodomain;
    };

    inline float randomFloat(float min, float max)
    {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<float> dis(min, max);
        return dis(gen);
    };

    inline void translateSpotLights(SpotLight *spotLights, size_t size, float xMin, float xMax, float yMin, float yMax, float zMin, float zMax)
    {
        for (size_t i = 0; i < size; i++)
        {
            auto spotLight = spotLights[i];
            float xTranslation = randomFloat(xMin, xMax);
            float yTranslation = randomFloat(yMin, yMax);
            float zTranslation = randomFloat(zMin, zMax);

            glm::vec3 translationVector(xTranslation, yTranslation, zTranslation);
            spotLight._spot.translateMesh(translationVector, 0);
            spotLight.position += translationVector;
        }
    }

    inline glm::vec3 randomVec3(float min, float max)
    {
        return glm::vec3(randomFloat(min, max), randomFloat(min, max), randomFloat(min, max));
    }

    // Fonction pour créer un tableau de 'Material' de taille spécifiée avec des champs glm::vec3 aléatoires
    inline void generateMaterials(std::vector<Geometry::Material> &materials, size_t size, float minVal = 0.3f, float maxVal = 1.0f)
    {
        materials.resize(size);

        for (auto &material : materials)
        {
            material.m_Ka = randomVec3(minVal, maxVal);
            material.m_Kd = randomVec3(minVal, maxVal);
            material.m_Ks = randomVec3(minVal, maxVal);
            material.m_Tr = randomVec3(minVal, maxVal);
            material.m_Le = randomVec3(minVal, maxVal);

            std::cout << "Ambiant = " << material.m_Ka << " Diffuse = " << material.m_Kd << " Spec = " << material.m_Ks << std::endl;

            material.m_Shininess = randomFloat(0.0f, 128.0f);
            material.m_RefractionIndex = randomFloat(1.0f, 2.5f);
            material.m_Dissolve = randomFloat(0.0f, 1.0f);

            material.m_pKaMap = nullptr;
            material.m_pKdMap = nullptr;
            material.m_pKsMap = nullptr;
            material.m_pNormalMap = nullptr;
        }
    }
};
