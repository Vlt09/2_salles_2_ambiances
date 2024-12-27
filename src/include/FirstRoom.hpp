#pragma once
#include "Room.hpp"
#include "Sphere.hpp"
#include "Cube.hpp"

class FirstRoom
{
    struct GlowStoneProg
    {
        Cube _glowStone;
        Room::UniformVariable _uniformVariable;
        glimac::Program _program;

        GlowStoneProg() : _glowStone(3)
        {
            _program = glimac::loadProgram("/home/valentin/m2/opengl/2_salles_2_ambiances/src/shaders/3D.vs.glsl",
                                           "/home/valentin/m2/opengl/2_salles_2_ambiances/src/shaders/pointlight.fs.glsl");

            _uniformVariable.uMVPMatrix = glGetUniformLocation(_program.getGLId(), "uMVPMatrix");
            _uniformVariable.uMVMatrix = glGetUniformLocation(_program.getGLId(), "uMVMatrix");
            _uniformVariable.uNormalMatrix = glGetUniformLocation(_program.getGLId(), "uNormalMatrix");
            _uniformVariable.uTexLoc = glGetUniformLocation(_program.getGLId(), "uTexture");
            _uniformVariable.uKd = glGetUniformLocation(_program.getGLId(), "uKd");
            _uniformVariable.uKs = glGetUniformLocation(_program.getGLId(), "uKs");
            _uniformVariable.uShininess = glGetUniformLocation(_program.getGLId(), "uShininess");
            _uniformVariable.uLightDir_vs = glGetUniformLocation(_program.getGLId(), "uLightPos_vs");
            _uniformVariable.uLightIntensity = glGetUniformLocation(_program.getGLId(), "uLightIntensity");

            _glowStone.initTexture("/home/valentin/m2/opengl/2_salles_2_ambiances/src/assets/glow_stone.jpg");
        };
    };

    struct SpotLight
    {
        Sphere _spot;

        glm::vec3 position;
        glm::vec3 direction;
        glm::vec3 intensity;

        float cutoff, exponent;

        SpotLight()
        {
        }

        SpotLight(Sphere spot, glm::vec3 pos, glm::vec3 dir, glm::vec3 inten, float cut, float exp)
            : _spot(spot), position(pos), direction(dir), intensity(inten), cutoff(cut), exponent(exp)
        {
        }
    };

private:
    unsigned short _lightFlag = 1; // Enable or Disable light process

    Room _box;
    SpotLight _spotLight;

    GlowStoneProg _glowStoneProg;

    Geometry _torch;

    glm::vec3 _spotPosition;

    glm::vec3 _lightPos;

public:
    Geometry::Material _spotMaterial;
    Geometry::Material _boxMaterial;

    FirstRoom() : _spotLight(
                      Sphere(),
                      glm::vec3(0.0f, 5.0f, 0.0f),
                      glm::vec3(0.0f, -1.0f, 0.0f),
                      glm::vec3(1.0f, 1.0f, 1.0f),
                      20.0f,
                      2.0f)

    {
        _spotMaterial.m_Ka = glm::vec3(1.0f, 1.0f, 0.0f);
        _spotMaterial.m_Kd = glm::vec3(1.0f, 1.0f, 0.0f);
        _spotMaterial.m_Ks = glm::vec3(1.0f, 1.0f, 0.0f);
        _spotMaterial.m_Tr = glm::vec3(0.0f, 0.0f, 0.0f);
        _spotMaterial.m_Le = glm::vec3(1.0f, 1.0f, 0.0f);
        _spotMaterial.m_Shininess = 128.f;
        _spotMaterial.m_RefractionIndex = 1.f;
        _spotMaterial.m_Dissolve = 1.0f;

        _boxMaterial.m_Kd = glm::vec3(0.4f, 0.3f, 0.2f);
        _boxMaterial.m_Ks = glm::vec3(0.05f, 0.05f, 0.05f);
        _boxMaterial.m_Shininess = 2.0f;

        _spotLight._spot.initSphere(1, 32, 16, _spotMaterial);
    }

    Sphere &getSpot()
    {
        return _spotLight._spot;
    }

    Room &getBox()
    {
        return _box;
    }

    GlowStoneProg &getGlowStoneProg()
    {
        return _glowStoneProg;
    }

    const Room::UniformVariable &getBoxUniformVariable()
    {
        return _box.getUniformVariable();
    }

    const Geometry &getTorch() const
    {
        return _torch;
    }

    unsigned short getLightFlag()
    {
        return _lightFlag;
    }

    void initFirstRoom(const glimac::FilePath &vsFilePath, const glimac::FilePath &fsFilePath, const glimac::FilePath &torchOBJFilePath,
                       const glimac::FilePath &torchMatFilePath, glm::vec3 cameraPos)
    {
        _box.initProgram(vsFilePath,
                         fsFilePath);

        _box.constructRoom(cameraPos, 1);

        _torch.loadOBJ(torchOBJFilePath, torchMatFilePath, true);
    }

    /**
     * @brief this method translate sphere that represents spot light and update
     * his position.
     */
    void translateSpotLight(const glm::vec3 &pos)
    {
        _spotLight._spot.translateModel(pos);
        _spotLight.position = pos;
    }

    const glm::vec3 &getLightDir()
    {
        return _spotPosition;
    }

    const SpotLight &getSpotLight()
    {
        return _spotLight;
    }

    void setLightPos(const glm::vec3 &pos)
    {
        _lightPos = pos;
    }

    const glm::vec3 &getLightPos()
    {
        return _lightPos;
    }
};
