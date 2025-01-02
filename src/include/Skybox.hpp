#pragma once
#include <glimac/FilePath.hpp>
#include <glimac/Program.hpp>
#include <string>
#include <vector>

class Skybox
{
private:
    GLuint _vao, _vbo, _uskybox, _uMVPMatrix;
    unsigned int _cubemapTexture;
    unsigned int loadCubemap(std::vector<std::string> &faces);

    glimac::Program _shader;

    void initSkybox();

public:
    Skybox(const glimac::FilePath &applicationDirPath)
    {
        initSkybox();

        std::vector<std::string> faces{
            applicationDirPath.dirPath() + "../src/assets/skybox/right.jpg",
            applicationDirPath.dirPath() + "../src/assets/skybox/left.jpg",
            applicationDirPath.dirPath() + "../src/assets/skybox/top.jpg",
            applicationDirPath.dirPath() + "../src/assets/skybox/bottom.jpg",
            applicationDirPath.dirPath() + "../src/assets/skybox/front.jpg",
            applicationDirPath.dirPath() + "../src/assets/skybox/back.jpg"};

        _cubemapTexture = loadCubemap(faces);
        _shader = glimac::loadProgram(applicationDirPath.dirPath() + "src/shaders/skybox.vs.glsl",
                                      applicationDirPath.dirPath() + "src/shaders/skybox.fs.glsl");

        _uskybox = glGetUniformLocation(_shader.getGLId(), "uskybox");
        _uMVPMatrix = glGetUniformLocation(_shader.getGLId(), "uMVPMatrix");
    }

    GLuint getShaderId()
    {
        return _shader.getGLId();
    }

    GLuint getUSkyboxLoc()
    {
        return _uskybox;
    }

    GLuint getUMVPLoc()
    {
        return _uMVPMatrix;
    }

    GLuint getVAO()
    {
        return _vao;
    }

    unsigned int getCubemapTexture()
    {
        return _cubemapTexture;
    }

    void use()
    {
        _shader.use();
    }
};
