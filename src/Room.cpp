#include "include/Room.hpp"

void Room::initProgram(const glimac::FilePath &vsFile, const glimac::FilePath &fsFile)
{
    _mProgram = glimac::loadProgram(vsFile, fsFile);

    uniformVariable.uMVPMatrix = glGetUniformLocation(_mProgram.getGLId(), "uMVPMatrix");
    uniformVariable.uMVMatrix = glGetUniformLocation(_mProgram.getGLId(), "uMVMatrix");
    uniformVariable.uNormalMatrix = glGetUniformLocation(_mProgram.getGLId(), "uNormalMatrix");
    uniformVariable.uModelMatrix = glGetUniformLocation(_mProgram.getGLId(), "uModelMatrix");

    uniformVariable.uTexLoc = glGetUniformLocation(_mProgram.getGLId(), "uTexture");
    uniformVariable.uKd = glGetUniformLocation(_mProgram.getGLId(), "uKd");
    uniformVariable.uKs = glGetUniformLocation(_mProgram.getGLId(), "uKs");
    uniformVariable.uShininess = glGetUniformLocation(_mProgram.getGLId(), "uShininess");
    uniformVariable.uLightDir_vs = glGetUniformLocation(_mProgram.getGLId(), "uLightDir_vs");
    uniformVariable.uLightPos_vs = glGetUniformLocation(_mProgram.getGLId(), "uLightPos_vs");
    uniformVariable.uLightIntensity = glGetUniformLocation(_mProgram.getGLId(), "uLightIntensity");
    uniformVariable.uSpotlightExponent = glGetUniformLocation(_mProgram.getGLId(), "uSpotlightExponent");
    uniformVariable.uSpotlightCutoff = glGetUniformLocation(_mProgram.getGLId(), "uSpotlightCutoff");
    uniformVariable.uSpotLight = glGetUniformLocation(_mProgram.getGLId(), "uSpotLight");
    uniformVariable.uActiveLight = glGetUniformLocation(_mProgram.getGLId(), "uActiveLight");
    uniformVariable.uHasTexture = glGetUniformLocation(_mProgram.getGLId(), "uHasTexture");
    uniformVariable.uCurrentSpotLightIdx = glGetUniformLocation(_mProgram.getGLId(), "uCurrentSpotLightIdx");
    uniformVariable.uUsePerlinNoise = glGetUniformLocation(_mProgram.getGLId(), "uUsePerlinNoise");

    // For the second room
    uniformVariable.uColor = glGetUniformLocation(_mProgram.getGLId(), "uColor");
    uniformVariable.uReflectance = glGetUniformLocation(_mProgram.getGLId(), "uReflectance");
    uniformVariable.uSpecularPower = glGetUniformLocation(_mProgram.getGLId(), "uSpecularPower");
    uniformVariable.uAmbientLight = glGetUniformLocation(_mProgram.getGLId(), "uAmbientLight");
    uniformVariable.uCamerapos = glGetUniformLocation(_mProgram.getGLId(), "uCameraPos");
    uniformVariable.uIsGlass = glGetUniformLocation(_mProgram.getGLId(), "uIsGlass");
}

void Room::addGroundAndFront(const glm::vec3 &cameraPos, std::vector<glimac::BBox3f> &bboxVector)
{
    float groundHeight = cameraPos.y - 0.15f;
    auto vertices = Quad::QuadVertices(1, 1);
    auto transformMat = glm::mat4(1);

    transformMat = glm::translate(transformMat, glm::vec3(cameraPos.x, groundHeight, cameraPos.z));
    transformMat = glm::scale(transformMat, glm::vec3(20.f, 1.f, 24.f));
    bboxVector.emplace_back(_bounds.addFromVertices(vertices, 0, transformMat));

    transformMat = glm::mat4(1);
    transformMat = glm::translate(transformMat, glm::vec3(cameraPos.x, cameraPos.y, cameraPos.z + 12.f));
    transformMat = glm::rotate(transformMat, glm::radians(90.f), glm::vec3(1.f, 0.f, 0.f));
    transformMat = glm::scale(transformMat, glm::vec3(20.f, 1.f, 24.f));

    bboxVector.emplace_back(_bounds.addFromVertices(vertices, 0, transformMat));

    transformMat = glm::mat4(1);
    transformMat = glm::translate(transformMat, glm::vec3(cameraPos.x, cameraPos.y, cameraPos.z - 12.f));
    transformMat = glm::rotate(transformMat, glm::radians(90.f), glm::vec3(1.f, 0.f, 0.f));
    transformMat = glm::scale(transformMat, glm::vec3(20.f, 1.f, 24.f));

    bboxVector.emplace_back(_bounds.addFromVertices(vertices, 0, transformMat));
}

void Room::addRightAndLeft(const glm::vec3 &cameraPos, float order, std::vector<glimac::BBox3f> &bboxVector)
{
    auto vertices = Quad::QuadVertices(1, 1);
    auto transformMat = glm::mat4(1);

    transformMat = glm::translate(transformMat, glm::vec3(cameraPos.x + (10.f * order), cameraPos.y + 2.f, cameraPos.z));
    transformMat = glm::rotate(transformMat, glm::radians(90.f), glm::vec3(0.f, 0.f, 1.f));
    transformMat = glm::scale(transformMat, glm::vec3(20.f, 1.f, 24.f));

    bboxVector.emplace_back(_bounds.addFromVertices(vertices, 0, transformMat));

    transformMat = glm::mat4(1);
    transformMat = glm::translate(transformMat, glm::vec3((cameraPos.x - (10.f * order)), cameraPos.y, cameraPos.z + 7.f));
    transformMat = glm::rotate(transformMat, glm::radians(90.f), glm::vec3(0.f, 0.f, 1.f));
    transformMat = glm::scale(transformMat, glm::vec3(20.f, 1.f, 10.f));
    bboxVector.emplace_back(_bounds.addFromVertices(vertices, 0, transformMat));

    transformMat = glm::mat4(1);
    transformMat = glm::translate(transformMat, glm::vec3(cameraPos.x - (10.f * order), cameraPos.y, cameraPos.z - 7.f));
    transformMat = glm::rotate(transformMat, glm::radians(90.f), glm::vec3(0.f, 0.f, 1.f));
    transformMat = glm::scale(transformMat, glm::vec3(20.f, 1.f, 10.f));
    bboxVector.emplace_back(_bounds.addFromVertices(vertices, 0, transformMat));
}

void Room::constructRoom(const glm::vec3 &cameraPos, float order, Geometry::Material &roomMat, std::vector<glimac::BBox3f> &bboxVector)
{
    addGroundAndFront(cameraPos, bboxVector);
    addRightAndLeft(cameraPos, order, bboxVector);
    _bounds.initMeshData();
    _bounds.addMaterial(roomMat);

    _bounds.initTexture("/home/valentin/m2/opengl/2_salles_2_ambiances/src/assets/minecraft_stonewall.png");
}
