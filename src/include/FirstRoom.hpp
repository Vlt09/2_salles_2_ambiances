#pragma once
#include "Room.hpp"
#include "Sphere.hpp"

class FirstRoom
{
private:
    Room _box;
    // Sphere _sun;

    Geometry::Material _boxMaterial;
    Geometry::Material _sunMaterial;

    Geometry _torch;

public:
    FirstRoom()
    {
    }
    void initFirstRoom(const glimac::FilePath &vsFilePath, const glimac::FilePath &fsFilePath, const glimac::FilePath &torchOBJFilePath,
                       const glimac::FilePath &torchMatFilePath, glm::vec3 cameraPos)
    {

        _sunMaterial.m_Ka = glm::vec3(1.0f, 1.0f, 0.0f);
        _sunMaterial.m_Kd = glm::vec3(1.0f, 1.0f, 0.0f);
        _sunMaterial.m_Ks = glm::vec3(1.0f, 1.0f, 0.0f);
        _sunMaterial.m_Tr = glm::vec3(0.0f, 0.0f, 0.0f);
        _sunMaterial.m_Le = glm::vec3(1.0f, 1.0f, 0.0f);
        _sunMaterial.m_Shininess = 128.f;
        _sunMaterial.m_RefractionIndex = 1.f;
        _sunMaterial.m_Dissolve = 1.0f;

        _boxMaterial.m_Kd = glm::vec3(0.4f, 0.3f, 0.2f);
        _boxMaterial.m_Ks = glm::vec3(0.05f, 0.05f, 0.05f);
        _boxMaterial.m_Shininess = 2.0f;

        _box.initProgram(vsFilePath,
                         fsFilePath);

        _box.constructRoom(cameraPos, 1);

        _torch.loadOBJ(torchOBJFilePath, torchMatFilePath, true);
    }
};
