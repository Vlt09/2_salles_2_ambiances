#pragma once
#include "Room.hpp"
#include "Sphere.hpp"
#include "Cube.hpp"
#include "Shape.hpp"
#include "Cylinder.hpp"
#include "Tore.hpp"
#include "Ring.hpp"
#include "Tube.hpp"
#include <map>

class FirstRoom
{

    static const unsigned int MAX_SPOT_LIGHT = 2;

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
    };

private:
    unsigned short _lightFlag = 1; // Enable or Disable light process

    Room _box;
    SpotLight _spotLight;
    SpotLight _spotLights[MAX_SPOT_LIGHT];
    SpotLightUniformVarLoc _spotLightUniformVarLoc[MAX_SPOT_LIGHT];

    // FirstRoom
    TwistCube _tCube;
    Cylinder _cy;

    // Second Room
    Ring _ring;
    WeirdTube _tube;

    std::vector<Quad> _glass;
    std::map<float, Quad *> _sortedGlass;

    glm::vec3 _lightPos;
    glm::vec3 _boxLightIntensity;

public:
    static const size_t MAX_GLASS = 3;
    Geometry::Material _spotMaterial[2];
    Geometry::Material _boxMaterial;

    GLuint feedbackBuffer;
    GLuint feedbackObject;

    FirstRoom() : _spotLight(
                      Sphere(),
                      glm::vec3(0.0f, 0.0f, 0.0f),
                      glm::vec3(0.0f, -1.0f, 0.0f),
                      glm::vec3(1.0f, 1.0f, 1.0f),
                      20.0f,
                      2.0f),
                  _tCube(5, 5, 5, 16, 8),
                  _ring(1.0f, 1.0f, 32, 32),
                  _tube(1.0f, 7.0f, 16, 16)
    {
        _spotMaterial[0].m_Ka = glm::vec3(0.0f, 0.0f, 0.5f);
        _spotMaterial[0].m_Kd = glm::vec3(0.0f, 0.0f, 0.5f);
        _spotMaterial[0].m_Ks = glm::vec3(0.0f, 0.0f, 0.5f);
        _spotMaterial[1].m_Ka = glm::vec3(0.5f, 0.0f, 0.0f);
        _spotMaterial[1].m_Kd = glm::vec3(0.5f, 0.0f, 0.1f);
        _spotMaterial[1].m_Ks = glm::vec3(0.5f, 0.0f, 0.1f);

        _spotMaterial[0].m_Tr = glm::vec3(0.0f, 0.0f, 0.0f);
        _spotMaterial[0].m_Le = glm::vec3(1.0f, 1.0f, 0.0f);
        _spotMaterial[0].m_Shininess = 128.f;
        _spotMaterial[0].m_RefractionIndex = 1.f;
        _spotMaterial[0].m_Dissolve = 1.0f;

        _boxMaterial.m_Kd = glm::vec3(0.3f, 0.3f, 0.4f);
        _boxMaterial.m_Ks = glm::vec3(0.2f, 0.2f, 0.1f);
        _boxMaterial.m_Shininess = 2.0f;

        _boxLightIntensity = glm::vec3(1.5f, 1.5f, 1.5f);

        _spotLights[0]._spot.initSphere(1, 32, 16, _spotMaterial[0]);
        _spotLights[1]._spot.initSphere(1, 32, 16, _spotMaterial[1]);

        _spotLight.intensity = glm::vec3(1.0f, 1.0f, 1.0f);
        _spotLight.cutoff = 2.f;
        _spotLights[0].intensity = glm::vec3(1.0f, 1.0f, 1.0f);
        _spotLights[0].cutoff = 25.f;

        _spotLights[1].intensity = glm::vec3(1.0f, 1.0f, 1.0f);
        _spotLights[1].cutoff = 25.f;

        _cy.initCylinder(1, 3, 16, 8, _boxMaterial);
    }

    Sphere &getSpot()
    {
        return _spotLight._spot;
    }

    Room &getBox()
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

    WeirdTube &getTube()
    {
        return _tube;
    }

    Ring &getRing()
    {
        return _ring;
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

    const glm::vec3 &getBoxLightIntensity()
    {
        return _boxLightIntensity;
    }

    const Room::UniformVariable &getBoxUniformVariable()
    {
        return _box.getUniformVariable();
    }

    unsigned short getLightFlag()
    {
        return _lightFlag;
    }

    /**
     * @brief this method translate sphere that represents spot light and update
     * his position.
     */
    void translateSpotLight(const glm::vec3 &pos, int idx)
    {
        _spotLights[idx]._spot.translateModel(pos);
        _spotLights[idx].position = pos;
    }

    void setSpotLightDirection(const glm::vec3 &targetPosition, int idx)
    {
        _spotLights[idx].direction = glm::normalize(targetPosition - _spotLights[idx].position);
    }

    const SpotLight &getSpotLight(int idx)
    {
        return _spotLight;
    }

    void setGlobalLightPos(const glm::vec3 &pos)
    {
        _lightPos = pos;
    }

    const glm::vec3 &getGlobalLightPos()
    {
        return _lightPos;
    }

    SpotLight *getSpotLightsData()
    {
        return &_spotLights[0];
    }

    SpotLightUniformVarLoc *getSpotLightsUniformVarLocData()
    {
        return &_spotLightUniformVarLoc[0];
    }

    void setSpotLightUniformLocations()
    {
        auto programID = _box.getProgramId();
        for (int i = 0; i < 2; ++i)
        {
            std::cout << "set spot light number " << i << std::endl;
            std::string baseName = "uSpotLights[" + std::to_string(i) + "]";

            _spotLightUniformVarLoc[i].position = glGetUniformLocation(programID, (baseName + "._position").c_str());
            _spotLightUniformVarLoc[i].direction = glGetUniformLocation(programID, (baseName + "._direction").c_str());
            _spotLightUniformVarLoc[i].lightIntensity = glGetUniformLocation(programID, (baseName + "._lightIntensity").c_str());
            _spotLightUniformVarLoc[i].lightPos = glGetUniformLocation(programID, (baseName + "._lightPos").c_str());
            _spotLightUniformVarLoc[i].cutoff = glGetUniformLocation(programID, (baseName + "._cutoff").c_str());
            _spotLightUniformVarLoc[i].exponent = glGetUniformLocation(programID, (baseName + "._exponent").c_str());
            _spotLightUniformVarLoc[i].m_Kd = glGetUniformLocation(programID, (baseName + ".m_Kd").c_str());
            _spotLightUniformVarLoc[i].m_Ks = glGetUniformLocation(programID, (baseName + ".m_Ks").c_str());
            _spotLightUniformVarLoc[i].m_Ka = glGetUniformLocation(programID, (baseName + ".m_Ka").c_str());

            if (_spotLightUniformVarLoc[i].position == -1)
                std::cerr << "Error: Could not find " << baseName << "._position" << std::endl;
            if (_spotLightUniformVarLoc[i].direction == -1)
                std::cerr << "Error: Could not find " << baseName << "._direction" << std::endl;
            if (_spotLightUniformVarLoc[i].lightIntensity == -1)
                std::cerr << "Error: Could not find " << baseName << "._lightIntensity" << std::endl;
            if (_spotLightUniformVarLoc[i].lightPos == -1)
                std::cerr << "Error: Could not find " << baseName << "._lightPos" << std::endl;
            if (_spotLightUniformVarLoc[i].cutoff == -1)
                std::cerr << "Error: Could not find " << baseName << "._spotlightCutoff" << std::endl;
            if (_spotLightUniformVarLoc[i].exponent == -1)
                std::cerr << "Error: Could not find " << baseName << "._spotlightExponent" << std::endl;
            if (_spotLightUniformVarLoc[i].m_Kd == -1)
                std::cerr << "Error: Could not find " << baseName << "m_Kd" << std::endl;
            if (_spotLightUniformVarLoc[i].m_Ks == -1)
                std::cerr << "Error: Could not find " << baseName << "m_Ks" << std::endl;
            if (_spotLightUniformVarLoc[i].m_Ka == -1)
                std::cerr << "Error: Could not find " << baseName << "m_Ka" << std::endl;
        }
    }

    void moveSpot(int idX)
    {
        auto minX = -10.f;
        auto maxX = 10.f;
        auto minY = 5.f;
        auto maxY = 10.f;
        auto minZ = -12.f;
        auto maxZ = 12.f;

        auto spot = _spotLights[idX];
        auto time = glimac::getTime();

        // Calculer la nouvelle position en utilisant des fonctions trigonométriques
        float x = spot.amplitude * sinf((spot.frequency * spot.speed) * time);        // Mouvement en X
        float y = spot.amplitude * cosf((spot.frequency * spot.speed) * time);        // Mouvement en Y
        float z = spot.amplitude * sinf((spot.frequency * spot.speed) * time / 2.0f); // Mouvement en Z (modifié)

        // Appliquer des bornes aux déplacements pour s'assurer que la sphère reste dans les limites
        if (x < minX)
            x = minX;
        if (x > maxX)
            x = maxX;
        if (y < minY)
            y = minY;
        if (y > maxY)
            y = maxY;
        if (z < minZ)
            z = minZ;
        if (z > maxZ)
            z = maxZ;

        auto translation = glm::vec3(x, y, z);
        spot._spot.translateModel(translation);
        translateSpotLight(translation, idX);

        // spot.modelMatrix = glm::rotate(spot.modelMatrix, glm::radians(30.0f * time), glm::vec3(0.0f, 1.0f, 0.0f));

        // sphere.modelMatrix = glm::scale(sphere.modelMatrix, glm::vec3(1.0f, 1.0f, 1.0f));
    }

    void initFirstRoom(const glimac::FilePath &vsFilePath, const glimac::FilePath &fsFilePath, const glimac::FilePath &torchOBJFilePath,
                       const glimac::FilePath &torchMatFilePath, glm::vec3 cameraPos)
    {
        _box.initProgram(vsFilePath,
                         fsFilePath);

        _box.constructRoom(cameraPos, 1, _boxMaterial);

        _cy.translateModel(glm::vec3(cameraPos.x - 3.f, cameraPos.y + 2.f, cameraPos.z));

        _tCube._cube.translateModel(glm::vec3(cameraPos.x + 3.f, cameraPos.y + 2.f, cameraPos.z + 2.f));

        setSpotLightUniformLocations();

        glGenBuffers(1, &feedbackBuffer);
        glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, feedbackBuffer);
        glBufferData(GL_TRANSFORM_FEEDBACK_BUFFER, sizeof(glm::vec3) * _box.getBounds().getVertexCount(), nullptr, GL_STATIC_READ);

        glGenTransformFeedbacks(1, &feedbackObject);
        glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, feedbackObject);

        const char *varyings[] = {"vVertexPos"};
        glTransformFeedbackVaryings(_box.getProgramId(), 1, varyings, GL_INTERLEAVED_ATTRIBS);
    }

    void initSecondRoom(const glimac::FilePath &vsFilePath, const glimac::FilePath &fsFilePath, glm::vec3 cameraPos, glimac::FilePath applicationPath)
    {

        // Shift for the second room
        glm::vec3 shift = cameraPos;

        _box.initProgram(vsFilePath,
                         fsFilePath);

        _box.constructRoom(cameraPos, -1, _boxMaterial);

        // Init glasses
        for (int i = 0; i < MAX_GLASS; i++)
        {
            _glass.emplace_back(Quad(3, 3));
            _glass[i].initTexture(applicationPath.dirPath() + "../src/assets/glass.png");
            _glass[i].translateModel(glm::vec3(cameraPos.x, cameraPos.y + 3.f, cameraPos.z + 1.f + ((float)i * 1.5)));
            _glass[i].rotateModel(glm::radians(90.f), glm::vec3(1., 0., 0.));
        }

        // Sort Glass using distance of an object from the viewer's perspective
        for (unsigned int i = 0; i < MAX_GLASS; i++)
        {
            auto glassPos = glm::vec3(_glass[i].getMeshBuffer()[0]._transform * glm::vec4(_glass[i].getVertexBuffer()[0].m_Position, 1.0));
            float distance = glm::length(cameraPos - glassPos);
            _sortedGlass[distance] = &_glass[i];
        }

        // Place other object
        _ring.translateModel(glm::vec3(shift.x, shift.y + 1.5f, shift.z - 10.f));
        _tube.translateModel(glm::vec3(shift.x - 5.f, shift.y + 3.f, shift.z));
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

        auto vVertexPos = positions[30];
        auto spotLight = _spotLight.position;
        auto lightToPixel = glm::normalize(vVertexPos - spotLight);
        auto dir = glm::normalize(glm::vec3(0, -1, 0));
        float spotFactor = glm::dot(lightToPixel, dir);

        std::cout << "spotlight position = " << spotLight << "vVertexPos " << vVertexPos << " lightToPixel = " << lightToPixel << " spotFactor = " << spotFactor << std::endl;
        glUnmapBuffer(GL_TRANSFORM_FEEDBACK_BUFFER);
    }
};
