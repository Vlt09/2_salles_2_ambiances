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
    }

    void addGroundAndFront(const glm::vec3 &cameraPos)
    {
        float groundHeight = cameraPos.y - 0.15f;

        auto vertices = Quad::QuadVertices(24, 20);
        auto &groundMesh = _bounds.addFromVertices(vertices);
        groundMesh._transform = glm::translate(groundMesh._transform, glm::vec3(cameraPos.x, groundHeight, cameraPos.z));
        groundMesh.isTransform = true;

        auto vertices3 = Quad::QuadVertices(10, 20);
        auto &frontWallMesh = _bounds.addFromVertices(vertices3);

        frontWallMesh._transform = glm::translate(frontWallMesh._transform, glm::vec3(cameraPos.x, cameraPos.y, cameraPos.z + 10.f));
        frontWallMesh._transform = glm::rotate(frontWallMesh._transform, glm::radians(90.f), glm::vec3(1.f, 0.f, 0.f));
        frontWallMesh.isTransform = true;

        auto vertices2 = Quad::QuadVertices(24, 10);
        auto &backWallMesh = _bounds.addFromVertices(vertices2);

        std::cout << "transform matrix = " << backWallMesh._transform << std::endl;

        backWallMesh._transform = glm::translate(backWallMesh._transform, glm::vec3(cameraPos.x - 10.f, cameraPos.y /*+ 4.f*/, cameraPos.z));
        backWallMesh._transform = glm::rotate(backWallMesh._transform, glm::radians(90.f), glm::vec3(0.f, 0.f, 1.f));
        backWallMesh._transform = glm::rotate(backWallMesh._transform, glm::radians(90.f), glm::vec3(1.f, 0.f, 0.f));
        backWallMesh.isTransform = true;
    }

    void addRightAndLeft(const glm::vec3 &cameraPos)
    {

        auto vertices = Quad::QuadVertices(24, 10);
        auto &leftWallMesh = _bounds.addFromVertices(vertices);

        leftWallMesh._transform = glm::rotate(leftWallMesh._transform, glm::radians(90.f), glm::vec3(0.f, 0.f, 1.f));
        leftWallMesh._transform = glm::translate(leftWallMesh._transform, glm::vec3(cameraPos.x, cameraPos.y - 10.f, cameraPos.z));

        leftWallMesh.isTransform = true;

        auto vertices2 = Quad::QuadVertices(10, 10);
        auto vertices3 = Quad::QuadVertices(10, 10);

        auto &halfRightWallMesh1 = _bounds.addFromVertices(vertices2);

        halfRightWallMesh1._transform = glm::rotate(halfRightWallMesh1._transform, glm::radians(90.f), glm::vec3(0.f, 0.f, 1.f));
        halfRightWallMesh1._transform = glm::translate(halfRightWallMesh1._transform, glm::vec3(cameraPos.x, cameraPos.y + 10.f, cameraPos.z + 7.f));
        halfRightWallMesh1.isTransform = true;

        auto &halfRightWallMesh2 = _bounds.addFromVertices(vertices3);

        halfRightWallMesh2._transform = glm::rotate(halfRightWallMesh2._transform, glm::radians(90.f), glm::vec3(0.f, 0.f, 1.f));
        halfRightWallMesh2._transform = glm::translate(halfRightWallMesh2._transform, glm::vec3(cameraPos.x, cameraPos.y + 10.f, cameraPos.z - 7.f));
        halfRightWallMesh2.isTransform = true;
    }

    void constructRoom(const glm::vec3 &cameraPos)
    {
        addGroundAndFront(cameraPos);
        addRightAndLeft(cameraPos);
        _bounds.initMeshData();
    }

private:
    std::list<Geometry> _objects;
    glimac::Program _mProgram;
    UniformMatrix uniformMatrix;

    Geometry _bounds;
};
