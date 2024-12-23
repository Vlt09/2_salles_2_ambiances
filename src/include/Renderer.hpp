#pragma once
#include "Camera.hpp"
#include "Scene.hpp"
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <glimac/Sphere.hpp>
#include <glimac/glm.hpp>
#include <glimac/Program.hpp>
#include <glimac/FilePath.hpp>
#include <glimac/Image.hpp>
#include <glimac/getTime.hpp>
#include <glimac/Geometry.hpp>

class Renderer
{
private:
    Camera camera;

    glimac::Program shaderProgramm;
    glm::mat4 projectionMatrix;
    glm::mat4 viewMatrix;

public:
    Renderer(/* args */);
    ~Renderer();

    void render(const glimac::Geometry &object, const glm::mat4 &modelMatrix);
    void renderScene(const Scene &scene);
};
