#pragma once
#include <list>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Quad.hpp"
#include "Geometry.hpp"
#include <glimac/Program.hpp>

class Room
{
public:
    struct UniformMatrix
    {
        GLint uMVPMatrix;
        GLint uMVMatrix;
        GLint uNormalMatrix;
    };

    const Geometry &getBounds() const
    {
        return _bounds;
    }

    Room()
    {
        addGround();
    }

private:
    std::list<Geometry> _objects;
    glimac::Program _mProgram;
    UniformMatrix uniformMatrix;

    Geometry _bounds;

    void addGround()
    {
        // auto vertices = Quad::QuadVertices(50, 50);
        // auto groundMesh = _bounds.addFromVertices(vertices);
        //_bounds.rotateModel(90.f, glm::vec3(1.f, 0.f, 0.f));

        auto vertices2 = Quad::QuadVertices(24, 20);
        auto &leftWallMesh = _bounds.addFromVertices(vertices2);
        // leftWallMesh._transform = glm::translate(leftWallMesh._transform, glm::vec3(0.f, 0.f, -5.f));
        // leftWallMesh._transform = glm::rotate(leftWallMesh._transform, 90.f, glm::vec3(1.f, 0.f, 0.f));
        leftWallMesh.isTransform = true;
    }
};
