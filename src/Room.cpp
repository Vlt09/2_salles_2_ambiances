#include "include/Room.hpp"

void Room::setSpotLightUniformLocations()
{
    auto programID = _box.getProgramId();
    for (int i = 0; i < MAX_SPOT_LIGHT; ++i)
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

void Room::moveSpot(int idX)
{
    auto minX = -9.f;
    auto maxX = 9.f;
    auto minY = 5.f;
    auto maxY = 10.f;
    auto minZ = -11.f;
    auto maxZ = 11.f;

    auto spot = _spotLights[idX];
    auto time = glimac::getTime();

    // Calculer la nouvelle position en utilisant des fonctions trigonométriques
    float x = spot.amplitude * sinf((spot.frequency * spot.speed) * time);
    float y = spot.amplitude * cosf((spot.frequency * spot.speed) * time);
    float z = spot.amplitude * sinf((spot.frequency * spot.speed) * time / 2.0f);

    auto mapX = Utils::linearMapping(x, -spot.amplitude, spot.amplitude, minX, maxX);
    auto mapY = Utils::linearMapping(y, -spot.amplitude, spot.amplitude, minY, maxY);
    auto mapZ = Utils::linearMapping(z, -spot.amplitude, spot.amplitude, minZ, maxZ);

    auto translation = glm::vec3(mapX, mapY, mapZ);

    translateSpotLight(translation, idX);
}

void Room::initFirstRoom(const glimac::FilePath &vsFilePath, const glimac::FilePath &fsFilePath,
                         glm::vec3 cameraPos, std::vector<glimac::BBox3f> &bboxVector, glimac::FilePath &applicationFilePath)
{
    _box.initProgram(vsFilePath,
                     fsFilePath);

    _box.constructRoom(cameraPos, 1, _boxMaterial, bboxVector, applicationFilePath);

    initSpotLight();

    _tCube._cube.initTexture(applicationFilePath.dirPath() + "../src/assets/fract_perlin_noise.jpg");
    _cy.initCylinder(1, 3, 16, 8, _boxMaterial);
    _cy.translateModel(glm::vec3(cameraPos.x - 3.f, cameraPos.y + 2.f, cameraPos.z + 5.f));

    _tCube._cube.translateModel(glm::vec3(cameraPos.x + 5.f, cameraPos.y + 2.5f, cameraPos.z + 2.f));

    _tore.translateMesh(glm::vec3(cameraPos.x + 5.f, cameraPos.y + 2.5f, cameraPos.z - 7.f), 0);

    setSpotLightUniformLocations();

    glGenBuffers(1, &feedbackBuffer);
    glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, feedbackBuffer);
    glBufferData(GL_TRANSFORM_FEEDBACK_BUFFER, sizeof(glm::vec3) * _box.getBounds().getVertexCount(), nullptr, GL_STATIC_READ);

    glGenTransformFeedbacks(1, &feedbackObject);
    glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, feedbackObject);

    const char *varyings[] = {"vVertexPos"};
    glTransformFeedbackVaryings(_box.getProgramId(), 1, varyings, GL_INTERLEAVED_ATTRIBS);
}

void Room::initSecondRoom(const glimac::FilePath &vsFilePath, const glimac::FilePath &fsFilePath, glm::vec3 cameraPos,
                          glimac::FilePath applicationPath, std::vector<glimac::BBox3f> &bboxVector)
{

    _box.initProgram(vsFilePath,
                     fsFilePath);

    _box.constructRoom(cameraPos, -1, _boxMaterial, bboxVector, applicationPath);

    auto id = _box.getProgramId();
    auto &uv = _box.getUniformVariable();

    // Place object
    _secondRoom._ring.translateModel(glm::vec3(cameraPos.x, cameraPos.y + 1.5f, cameraPos.z - 10.f));
    _secondRoom._tube.translateModel(glm::vec3(cameraPos.x - 5.f, cameraPos.y + 3.f, cameraPos.z));
    _secondRoom._disco.translateModel(glm::vec3(cameraPos.x, cameraPos.y + 3.f, cameraPos.z));

    _secondRoom.initComponent(id, glm::vec3(0, -1, 0), glm::vec3(cameraPos.x, cameraPos.y + 5.f, cameraPos.z));

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
}

void Room::clearBuffers()
{
    _tCube._cube.clearBuffers();
    _tCube._cone.clearBuffers();
    _cy.clearBuffers();
    _tore.clearBuffers();
    _secondRoom.clearBuffers();
}

void Room::printDebugBuff()
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
