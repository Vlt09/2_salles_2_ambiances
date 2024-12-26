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
        GLint uTexLoc;
    };

private:
    std::list<Geometry> _objects;
    glimac::Program _mProgram;
    UniformMatrix uniformMatrix;

    Geometry _bounds;

    /**
     * @brief Adds left and right wall meshes to the scene with specific transformations.
     *
     * This function creates four 3D wall meshes (a left wall and two half-right walls)
     * based on the given camera position and an order parameter. Each wall is represented
     * by a quad (1x1 unit), and the following transformations are applied:
     *
     * - **Translation**: Each wall is translated relative to the camera position and order.
     * - **Rotation**: Each wall is rotated 90 degrees around the Z-axis.
     * - **Scaling**: The left wall is scaled to a size of `(20, 1, 24)`, while the right walls
     *   are each scaled to a size of `(20, 1, 10)`.
     *
     * @param cameraPos The camera position in world space. Used to position the walls relative to the camera.
     * @param order An unsigned short integer that determines the positioning of the opening.
     *
     * @note The scaling values are defined according to the specifications of the project
     */
    void addRightAndLeft(const glm::vec3 &cameraPos, unsigned short order)
    {

        auto vertices = Quad::QuadVertices(1, 1);
        auto &leftWallMesh = _bounds.addFromVertices(vertices);

        leftWallMesh._transform = glm::translate(leftWallMesh._transform, glm::vec3(cameraPos.x + (10.f * order), cameraPos.y + 2.f, cameraPos.z));
        leftWallMesh._transform = glm::rotate(leftWallMesh._transform, glm::radians(90.f), glm::vec3(0.f, 0.f, 1.f));
        leftWallMesh._transform = glm::scale(leftWallMesh._transform, glm::vec3(20.f, 1.f, 24.f));

        leftWallMesh.isTransform = true;

        auto &halfRightWallMesh1 = _bounds.addFromVertices(vertices);

        halfRightWallMesh1._transform = glm::translate(halfRightWallMesh1._transform, glm::vec3(cameraPos.x - (10.f * order), cameraPos.y, cameraPos.z + 7.f));
        halfRightWallMesh1._transform = glm::rotate(halfRightWallMesh1._transform, glm::radians(90.f), glm::vec3(0.f, 0.f, 1.f));
        halfRightWallMesh1._transform = glm::scale(halfRightWallMesh1._transform, glm::vec3(20.f, 1.f, 10.f));

        halfRightWallMesh1.isTransform = true;

        auto &halfRightWallMesh2 = _bounds.addFromVertices(vertices);

        halfRightWallMesh2._transform = glm::translate(halfRightWallMesh2._transform, glm::vec3(cameraPos.x - (10.f * order), cameraPos.y, cameraPos.z - 7.f));
        halfRightWallMesh2._transform = glm::rotate(halfRightWallMesh2._transform, glm::radians(90.f), glm::vec3(0.f, 0.f, 1.f));
        halfRightWallMesh2._transform = glm::scale(halfRightWallMesh2._transform, glm::vec3(20.f, 1.f, 10.f));

        halfRightWallMesh2.isTransform = true;
    }

public:
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
        auto vertices = Quad::QuadVertices(1, 1);

        auto &groundMesh = _bounds.addFromVertices(vertices);
        groundMesh._transform = glm::translate(groundMesh._transform, glm::vec3(cameraPos.x, groundHeight, cameraPos.z));
        groundMesh._transform = glm::scale(groundMesh._transform, glm::vec3(20.f, 1.f, 24.f));
        groundMesh.isTransform = true;

        auto &frontWallMesh = _bounds.addFromVertices(vertices);

        frontWallMesh._transform = glm::translate(frontWallMesh._transform, glm::vec3(cameraPos.x, cameraPos.y, cameraPos.z + 12.f));
        frontWallMesh._transform = glm::rotate(frontWallMesh._transform, glm::radians(90.f), glm::vec3(1.f, 0.f, 0.f));
        frontWallMesh._transform = glm::scale(frontWallMesh._transform, glm::vec3(20.f, 1.f, 24.f));

        frontWallMesh.isTransform = true;

        auto &backWallMesh = _bounds.addFromVertices(vertices);

        backWallMesh._transform = glm::translate(backWallMesh._transform, glm::vec3(cameraPos.x, cameraPos.y, cameraPos.z - 12.f));

        backWallMesh._transform = glm::rotate(backWallMesh._transform, glm::radians(90.f), glm::vec3(1.f, 0.f, 0.f));
        backWallMesh._transform = glm::scale(backWallMesh._transform, glm::vec3(20.f, 1.f, 24.f));
        backWallMesh.isTransform = true;
    }

    void constructRoom(const glm::vec3 &cameraPos, unsigned short order)
    {
        addGroundAndFront(cameraPos);
        addRightAndLeft(cameraPos, order);
        _bounds.initMeshData();

        _bounds.initTexture("/home/valentin/m2/opengl/2_salles_2_ambiances/src/assets/minecraft_stonewall.png");
    }

    void displayVertices(const std::vector<Geometry::Vertex> &vertices)
    {
        for (const auto &vertex : vertices)
        {
            std::cout << "Vertex:" << std::endl;
            std::cout << "  Position: ("
                      << vertex.m_Position.x << ", "
                      << vertex.m_Position.y << ", "
                      << vertex.m_Position.z << ")" << std::endl;
            std::cout << "  Normal: ("
                      << vertex.m_Normal.x << ", "
                      << vertex.m_Normal.y << ", "
                      << vertex.m_Normal.z << ")" << std::endl;
            std::cout << "  Texture Coordinates: ("
                      << vertex.m_TexCoords.x << ", "
                      << vertex.m_TexCoords.y << ")" << std::endl;
            std::cout << std::endl; // Add an empty line for better readability
        }
    }
};
