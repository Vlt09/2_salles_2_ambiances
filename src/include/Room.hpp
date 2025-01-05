#pragma once
#include "Box.hpp"
#include "Sphere.hpp"
#include "Cube.hpp"
#include "Shape.hpp"
#include "Cylinder.hpp"
#include "Tore.hpp"
#include "Ring.hpp"
#include "Tube.hpp"
#include "Utils.hpp"
#include <map>

class Room
{

public:
    static const unsigned int MAX_SPOT_LIGHT = 15;

    struct SecondRoomComposent
    {
        static const unsigned int MAX_DIR_LIGHT = 1;
        static const unsigned int MAX_POINT_LIGHT = 50;

        std::vector<Utils::DirectionalLightUniformVarLoc> _direcLightLoc;
        std::vector<Utils::DirectionalLight> _dirLight;

        std::vector<Utils::PointLightUniformVarLoc> _pointLightLoc;
        std::vector<Utils::PointLight> _pointLight;

        Ring _ring;
        WeirdTube _tube;
        Sphere _disco;

        glm::vec3 _ambientLight = glm::vec3(0.3f, 0.3f, 0.3f);
        float _specularPower = 8.f;

        // Material value
        glm::vec3 _matColor = glm::vec3(0.2f, 0.5f, 0.5f);
        float _reflectance = 1.f;

        SecondRoomComposent() : _ring(1.0f, 1.0f, 32, 32),
                                _tube(1.0f, 7.0f, 16, 16),
                                _disco(1.0f, 32, 16),
                                _direcLightLoc(MAX_DIR_LIGHT),
                                _pointLightLoc(MAX_POINT_LIGHT)
        {
        }

        SecondRoomComposent(GLuint progID) : _ring(1.0f, 1.0f, 32, 32),
                                             _tube(1.0f, 7.0f, 16, 16)
        {
            Utils::setDirectionalLightUniformLocations(progID, _direcLightLoc.data(), MAX_DIR_LIGHT);
            Utils::setPointLightUniformLocations(progID, _pointLightLoc.data(), MAX_POINT_LIGHT);
        }

        void initComponent(GLuint progID, glm::vec3 dirLightDir, glm::vec3 pointLightPos)
        {
            Utils::setDirectionalLightUniformLocations(progID, _direcLightLoc.data(), MAX_DIR_LIGHT);
            Utils::setPointLightUniformLocations(progID, _pointLightLoc.data(), MAX_POINT_LIGHT);
            auto &transform = _disco.getMeshBuffer()[0]._transform;

            _dirLight.emplace_back(Utils::DirectionalLight(
                glm::vec3(1),
                dirLightDir));

            for (size_t i = 0; i < MAX_POINT_LIGHT; i++)
            {
                glm::vec3 position = _disco.getPointOnSphere(i, MAX_POINT_LIGHT);
                auto posWS = glm::vec3(transform * glm::vec4(position, 1.0));

                _pointLight.emplace_back(Utils::PointLight(
                    Utils::randomVec3(0.2, 0.5),
                    posWS,
                    1.f,
                    0.f,
                    0.f,
                    0.5f));
            }
        }

        void rotateDiscoAndLight()
        {
            auto time = glimac::getTime();
            float speed = 2.0f;

            float angle = glm::radians(speed * time);
            _disco.rotateMesh(angle, glm::vec3(0., 1., 0.), 0);

            auto transform = _disco.getMeshBuffer()[0]._transform;
            for (size_t i = 0; i < MAX_POINT_LIGHT; i++)
            {
                _pointLight[i]._position = glm::vec3(transform * glm::vec4(_pointLight[i]._position, 1.0));

                _pointLight[i]._intensity = 0.5f + 0.5f * sin(time + glm::linearRand(0.0f, 0.8f));
                _pointLight[i]._color = glm::vec3(
                    0.5f + 0.5f * sin(time + glm::length(_pointLight[i]._position) * 0.1f),
                    0.5f + 0.5f * cos(time + _pointLight[i]._position.x * 0.1f),
                    0.5f + 0.5f * sin(time + _pointLight[i]._position.y * 0.1f));
            }
        }

        void clearBuffers()
        {
            _ring.clearBuffers();
            _tube.clearBuffers();
            _disco.clearBuffers();
        }
    };

private:
    Box _box;
    Utils::SpotLight _spotLight;
    Utils::SpotLight _spotLights[MAX_SPOT_LIGHT];
    Utils::SpotLightUniformVarLoc _spotLightUniformVarLoc[MAX_SPOT_LIGHT];

    // Room
    TwistCube _tCube;
    Cylinder _cy;
    Tore _tore;

    // Second Box
    SecondRoomComposent _secondRoom;

    std::vector<Quad> _glass;
    std::map<float, Quad *> _sortedGlass;

    glm::vec3 _lightPos;
    glm::vec3 _boxLightIntensity;

    void initSpotLight()
    {
        Utils::generateMaterials(_spotMaterials, MAX_SPOT_LIGHT, 0.0f, 1.0f);

        for (size_t i = 0; i < MAX_SPOT_LIGHT; i++)
        {
            glm::vec3 randomPosition = Utils::randomVec3(-10.0f, 10.0f);
            glm::vec3 randomDirection = glm::normalize(Utils::randomVec3(-1.0f, 1.0f));
            glm::vec3 randomIntensity = Utils::randomVec3(0.5f, 1.5f);

            _spotLights[i] = Utils::SpotLight(
                Sphere(),
                randomPosition,
                randomDirection,
                randomIntensity,
                Utils::randomFloat(10.f, 30.f),
                1.0f,
                Utils::randomFloat(0.1, 0.9),
                Utils::randomFloat(1.f, 15.f));

            _spotLights[i]._spot.initSphere(0.5f, 32, 16, _spotMaterials[i]);
        }
    }

public:
    static const size_t MAX_GLASS = 3;

    std::vector<Geometry::Material> _spotMaterials;

    Geometry::Material _boxMaterial;

    GLuint feedbackBuffer;
    GLuint feedbackObject;

    Room() : _spotLight(
                 Sphere(),
                 glm::vec3(0.0f, 0.0f, 0.0f),
                 glm::vec3(0.0f, -1.0f, 0.0f),
                 glm::vec3(1.0f, 1.0f, 1.0f),
                 20.0f,
                 2.0f),
             _tCube(5, 5, 5, 16, 8),
             _tore(2.f, 2.f)
    {

        _boxMaterial.m_Ka = glm::vec3(0.7f, 0.7f, 0.7f);
        _boxMaterial.m_Kd = glm::vec3(0.3f, 0.3f, 0.4f);
        _boxMaterial.m_Ks = glm::vec3(0.2f, 0.2f, 0.1f);
        _boxMaterial.m_Shininess = 2.0f;

        _boxLightIntensity = glm::vec3(1.5f, 1.5f, 1.5f);

        _spotLight.intensity = glm::vec3(1.0f, 1.0f, 1.0f);
        _spotLight.cutoff = 2.f;
    }

    Sphere &getSpot()
    {
        return _spotLight._spot;
    }

    Box &getBox()
    {
        return _box;
    }

    TwistCube &getTwistCube()
    {
        return _tCube;
    }

    Cylinder &getCylinder()
    {
        return _cy;
    }

    Tore &getTore()
    {
        return _tore;
    }

    Quad *getGlassData()
    {
        return &_glass[0];
    }

    std::map<float, Quad *> &getSortedGlass()
    {
        return _sortedGlass;
    }

    size_t glassSize()
    {
        return _glass.size();
    }

    const Box::UniformVariable &getBoxUniformVariable()
    {
        return _box.getUniformVariable();
    }

    const std::vector<Geometry::Material> &getSpotLightMaterials()
    {
        return _spotMaterials;
    }

    /**
     * @brief this method translate sphere that represents spot light and update
     * his position.
     */
    void translateSpotLight(const glm::vec3 &pos, int idx)
    {
        _spotLights[idx]._spot.translateMeshWithIdentity(pos, 0);
        _spotLights[idx].position = pos;
    }

    void setSpotLightDirection(const glm::vec3 &targetPosition, int idx)
    {
        _spotLights[idx].direction = glm::normalize(targetPosition - _spotLights[idx].position);
    }

    void setGlobalLightPos(const glm::vec3 &pos)
    {
        _lightPos = pos;
    }

    const glm::vec3 &getGlobalLightPos()
    {
        return _lightPos;
    }

    Utils::SpotLight *getSpotLightsData()
    {
        return &_spotLights[0];
    }

    Utils::SpotLightUniformVarLoc *getSpotLightsUniformVarLocData()
    {
        return &_spotLightUniformVarLoc[0];
    }

    SecondRoomComposent &getSecondRoom()
    {
        return _secondRoom;
    }

    void setSpotLightUniformLocations();

    void moveSpot(int idX);

    void initFirstRoom(const glimac::FilePath &vsFilePath, const glimac::FilePath &fsFilePath,
                       glm::vec3 cameraPos, std::vector<glimac::BBox3f> &bboxVector, glimac::FilePath &applicationFilePath);

    void initSecondRoom(const glimac::FilePath &vsFilePath, const glimac::FilePath &fsFilePath, glm::vec3 cameraPos,
                        glimac::FilePath applicationPath, std::vector<glimac::BBox3f> &bboxVector);

    void clearBuffers();

    void printDebugBuff();
};
