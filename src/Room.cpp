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
}

void Room::addGroundAndFront(const glm::vec3 &cameraPos)
{
    float groundHeight = cameraPos.y - 0.15f;
    auto vertices = Quad::QuadVertices(1, 1);

    auto &groundMesh = _bounds.addFromVertices(vertices, 0);
    groundMesh._transform = glm::translate(groundMesh._transform, glm::vec3(cameraPos.x, groundHeight, cameraPos.z));
    groundMesh._transform = glm::scale(groundMesh._transform, glm::vec3(20.f, 1.f, 24.f));
    groundMesh.isTransform = true;

    auto &frontWallMesh = _bounds.addFromVertices(vertices, 0);

    frontWallMesh._transform = glm::translate(frontWallMesh._transform, glm::vec3(cameraPos.x, cameraPos.y, cameraPos.z + 12.f));
    frontWallMesh._transform = glm::rotate(frontWallMesh._transform, glm::radians(90.f), glm::vec3(1.f, 0.f, 0.f));
    frontWallMesh._transform = glm::scale(frontWallMesh._transform, glm::vec3(20.f, 1.f, 24.f));

    frontWallMesh.isTransform = true;

    auto &backWallMesh = _bounds.addFromVertices(vertices, 0);

    backWallMesh._transform = glm::translate(backWallMesh._transform, glm::vec3(cameraPos.x, cameraPos.y, cameraPos.z - 12.f));

    backWallMesh._transform = glm::rotate(backWallMesh._transform, glm::radians(90.f), glm::vec3(1.f, 0.f, 0.f));
    backWallMesh._transform = glm::scale(backWallMesh._transform, glm::vec3(20.f, 1.f, 24.f));
    backWallMesh.isTransform = true;
}

void Room::addRightAndLeft(const glm::vec3 &cameraPos, float order)
{
    auto vertices = Quad::QuadVertices(1, 1);
    auto &leftWallMesh = _bounds.addFromVertices(vertices, 0);

    leftWallMesh._transform = glm::translate(leftWallMesh._transform, glm::vec3(cameraPos.x + (10.f * order), cameraPos.y + 2.f, cameraPos.z));
    leftWallMesh._transform = glm::rotate(leftWallMesh._transform, glm::radians(90.f), glm::vec3(0.f, 0.f, 1.f));
    leftWallMesh._transform = glm::scale(leftWallMesh._transform, glm::vec3(20.f, 1.f, 24.f));

    leftWallMesh.isTransform = true;

    auto &halfRightWallMesh1 = _bounds.addFromVertices(vertices, 0);

    halfRightWallMesh1._transform = glm::translate(halfRightWallMesh1._transform, glm::vec3((cameraPos.x - (10.f * order)), cameraPos.y, cameraPos.z + 7.f));
    halfRightWallMesh1._transform = glm::rotate(halfRightWallMesh1._transform, glm::radians(90.f), glm::vec3(0.f, 0.f, 1.f));
    halfRightWallMesh1._transform = glm::scale(halfRightWallMesh1._transform, glm::vec3(20.f, 1.f, 10.f));

    halfRightWallMesh1.isTransform = true;

    auto &halfRightWallMesh2 = _bounds.addFromVertices(vertices, 0);

    halfRightWallMesh2._transform = glm::translate(halfRightWallMesh2._transform, glm::vec3(cameraPos.x - (10.f * order), cameraPos.y, cameraPos.z - 7.f));
    halfRightWallMesh2._transform = glm::rotate(halfRightWallMesh2._transform, glm::radians(90.f), glm::vec3(0.f, 0.f, 1.f));
    halfRightWallMesh2._transform = glm::scale(halfRightWallMesh2._transform, glm::vec3(20.f, 1.f, 10.f));

    halfRightWallMesh2.isTransform = true;
}

void Room::constructRoom(const glm::vec3 &cameraPos, float order, Geometry::Material &roomMat)
{
    addGroundAndFront(cameraPos);
    addRightAndLeft(cameraPos, order);
    _bounds.initMeshData();
    _bounds.addMaterial(roomMat);

    _bounds.initTexture("/home/valentin/m2/opengl/2_salles_2_ambiances/src/assets/minecraft_stonewall.png");
}
