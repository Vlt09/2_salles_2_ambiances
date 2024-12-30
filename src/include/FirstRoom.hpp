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

        GlowStoneProg() : _glowStone(3) {
                              // _program = glimac::loadProgram("/home/valentin/m2/opengl/2_salles_2_ambiances/src/shaders/3D.vs.glsl",
                              //                                "/home/valentin/m2/opengl/2_salles_2_ambiances/src/shaders/pointlight.fs.glsl");

                              // _uniformVariable.uMVPMatrix = glGetUniformLocation(_program.getGLId(), "uMVPMatrix");
                              // _uniformVariable.uMVMatrix = glGetUniformLocation(_program.getGLId(), "uMVMatrix");
                              // _uniformVariable.uNormalMatrix = glGetUniformLocation(_program.getGLId(), "uNormalMatrix");
                              // _uniformVariable.uTexLoc = glGetUniformLocation(_program.getGLId(), "uTexture");
                              // _uniformVariable.uKd = glGetUniformLocation(_program.getGLId(), "uKd");
                              // _uniformVariable.uKs = glGetUniformLocation(_program.getGLId(), "uKs");
                              // _uniformVariable.uShininess = glGetUniformLocation(_program.getGLId(), "uShininess");
                              // _uniformVariable.uLightDir_vs = glGetUniformLocation(_program.getGLId(), "uLightPos_vs");
                              // _uniformVariable.uLightIntensity = glGetUniformLocation(_program.getGLId(), "uLightIntensity");

                              // _glowStone.initTexture("/home/valentin/m2/opengl/2_salles_2_ambiances/src/assets/glow_stone.jpg");
                          };
    };

    struct SpotLight
    {
        Sphere _spot;

        glm::vec3 position;
        glm::vec3 direction = glm::vec3(0., -1., 0.);
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

    glm::vec3 _boxLightIntensity;

public:
    Geometry::Material _spotMaterial;
    Geometry::Material _boxMaterial;

    GLuint feedbackBuffer;
    GLuint feedbackObject;

    FirstRoom() : _spotLight(
                      Sphere(),
                      glm::vec3(0.0f, 5.0f, 0.0f),
                      glm::vec3(0.0f, -1.0f, 0.0f),
                      glm::vec3(1.0f, 1.0f, 1.0f),
                      20.0f,
                      2.0f)

    {
        // _spotMaterial.m_Ka = glm::vec3(1.0f, 1.0f, 0.0f);
        // _spotMaterial.m_Kd = glm::vec3(1.0f, 1.0f, 0.0f);
        // _spotMaterial.m_Ks = glm::vec3(1.0f, 1.0f, 0.0f);
        _spotMaterial.m_Ka = glm::vec3(1.1f, 1.1f, 1.1f);
        _spotMaterial.m_Kd = glm::vec3(0.8f, 0.8f, 0.8f);
        _spotMaterial.m_Ks = glm::vec3(0.9f, 0.9f, 0.9f);

        _spotMaterial.m_Tr = glm::vec3(0.0f, 0.0f, 0.0f);
        _spotMaterial.m_Le = glm::vec3(1.0f, 1.0f, 0.0f);
        _spotMaterial.m_Shininess = 128.f;
        _spotMaterial.m_RefractionIndex = 1.f;
        _spotMaterial.m_Dissolve = 1.0f;

        _boxMaterial.m_Kd = glm::vec3(0.3f, 0.3f, 0.4f);
        _boxMaterial.m_Ks = glm::vec3(0.2f, 0.2f, 0.1f);
        _boxMaterial.m_Shininess = 2.0f;

        _spotLight._spot.initSphere(1, 32, 16, _spotMaterial);
        _boxLightIntensity = glm::vec3(0.5f, 0.5f, 0.5f);
        _spotLight.intensity = glm::vec3(1.0f, 1.0f, 1.0f);
        _spotLight.cutoff = 2.f;
    }

    Sphere &getSpot()
    {
        return _spotLight._spot;
    }

    Room &getBox()
    {
        return _box;
    }

    const glm::vec3 &getBoxLightIntensity()
    {
        return _boxLightIntensity;
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

        glGenBuffers(1, &feedbackBuffer);
        glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, feedbackBuffer);
        glBufferData(GL_TRANSFORM_FEEDBACK_BUFFER, sizeof(glm::vec3) * _box.getBounds().getVertexCount(), nullptr, GL_STATIC_READ);

        glGenTransformFeedbacks(1, &feedbackObject);
        glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, feedbackObject);

        const char *varyings[] = {"vVertexNormal"};
        glTransformFeedbackVaryings(_box.getProgramId(), 1, varyings, GL_INTERLEAVED_ATTRIBS);
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

    void printDebugBuff()
    {
        // Lire le buffer après le rendu
        glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, feedbackBuffer);
        glm::vec3 *positions = (glm::vec3 *)glMapBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, GL_READ_ONLY);
        // Accéder aux données et les afficher ou les enregistrer
        for (int i = 0; i < _box.getBounds().getVertexCount(); ++i)
        {
            std::cout << "Vertex " << i << " Position: " << positions[i].x << ", " << positions[i].y << ", " << positions[i].z << std::endl;
        }

        auto vVertexPos = positions[0];
        auto spotLight = _spotLight.position;
        auto lightToPixel = glm::normalize(vVertexPos - spotLight);
        auto dir = glm::normalize(glm::vec3(0, -1, 0));
        float spotFactor = glm::dot(lightToPixel, dir);

        std::cout << "lightToPixel = " << lightToPixel << " spotFactor = " << spotFactor << std::endl;
        glUnmapBuffer(GL_TRANSFORM_FEEDBACK_BUFFER);
    }
};
