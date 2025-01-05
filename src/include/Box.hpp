#pragma once
#include <list>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Quad.hpp"
#include "Geometry.hpp"
#include <glimac/Program.hpp>

class Box
{
public:
    struct UniformVariable
    {
        GLint uMVPMatrix;
        GLint uMVMatrix;
        GLint uNormalMatrix;
        GLint uModelMatrix;
        GLint uTexLoc;
        GLint uKa;
        GLint uKd;
        GLint uKs;
        GLint uShininess;
        GLint uLightDir_vs;
        GLint uLightPos_vs;
        GLint uLightIntensity;
        GLint uSpotlightCutoff;
        GLint uSpotlightExponent;
        GLint uSpotLight;
        GLint uActiveLight;
        GLint uHasTexture;
        GLint uCurrentSpotLightIdx;
        GLint uUsePerlinNoise;
        GLint uSpecularPower;
        GLint uAmbientLight;
        GLint uColor;
        GLint uReflectance;
        GLint uCamerapos;
        GLint uIsGlass;
    };

    const Geometry &getBounds() const
    {
        return _bounds;
    }

    Box()
    {
    }

    void initProgram(const glimac::FilePath &vsFile, const glimac::FilePath &fsFile);

    void addGroundAndFront(const glm::vec3 &cameraPos, std::vector<glimac::BBox3f> &bboxVector);

    void constructRoom(const glm::vec3 &cameraPos, float order, Geometry::Material &roomMat, std::vector<glimac::BBox3f> &bboxVector, const glimac::FilePath &appPath);

    const UniformVariable &getUniformVariable()
    {
        return uniformVariable;
    }

    glimac::Program &getProgram()
    {
        return _mProgram;
    }

    GLuint getProgramId()
    {
        return _mProgram.getGLId();
    }

private:
    std::list<Geometry> _objects;
    UniformVariable uniformVariable;
    glimac::Program _mProgram;

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
    void addRightAndLeft(const glm::vec3 &cameraPos, float order, std::vector<glimac::BBox3f> &bboxVector);
};
