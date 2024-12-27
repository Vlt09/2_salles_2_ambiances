#pragma once
#include "Geometry.hpp"
#include "Scene.hpp"
#include "Room.hpp"
#include "FirstRoom.hpp"
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <glimac/glm.hpp>
#include <glimac/Program.hpp>
#include <glimac/FilePath.hpp>
#include <glimac/Image.hpp>
#include <glimac/getTime.hpp>

class Renderer
{
private:
    glimac::Program _shaderProgram;
    glm::highp_mat4 _projectionMatrix;
    glm::mat4 _viewMatrix;

public:
    Renderer(/* args */);
    Renderer(const Renderer &renderer);

    Renderer(glm::highp_mat4 &projMatrix, glm::mat4 &viewMatrix);

    /**
     * @brief Renders a 3D object in the scene.
     *
     * This function takes a geometric object and renders it to the screen
     * by applying a model transformation matrix. If no model matrix is provided,
     * the identity matrix is used by default.
     *
     * @param object The 3D object in a Room to render. It should be an instance of a class derived from `glimac::Geometry`,
     *               which contains the necessary data for rendering (vertices, normals, etc.).
     *
     * @param uniformMatrix The struct which contains the uniform variable for the current Room.
     *
     * @param modelMatrix The model transformation matrix to apply to the object. By default, it is the identity matrix
     *                    (`glm::mat4(1)`) which doesn't modify the object
     *
     */
    void render(const Geometry &object, Room::UniformVariable uniformVariable);

    void render(const Geometry &object, Geometry::Material material, Room::UniformVariable uniformVariable);

    void render(const Geometry &object, Room::UniformVariable uniformVariable, glm::vec3 shift);

    void renderFirstRoom(FirstRoom &firstRoom);

    template <typename Func>
    void applyToAllMeshes(const std::vector<Geometry::Mesh> &meshes, Func &&func);

    void setMaterialAndLightingUniforms(const Room::UniformVariable &uniformVariable,
                                        const glm::vec3 &light_dir_vs,
                                        const glm::vec3 &light_pos_vs,
                                        const glm::vec3 &lightIntensity,
                                        const glm::vec3 &kd,
                                        const glm::vec3 &ks,
                                        float shininess);

    void setMatricesToShader(const Room::UniformVariable &uniformVariable,
                             const glm::mat4 &projectionMatrix,
                             const glm::mat4 &mvMatrix,
                             const glm::mat4 &normalMatrix);

    void renderScene(const Scene &scene);

    void setViewMatrix(glm::mat4 viewMatrix)
    {
        _viewMatrix = viewMatrix;
    }

    void setSpotLightUniform(const Room::UniformVariable &uniformVariable, const glm::vec3 &spotLight, float spotlightCutoff, float spotlightExponent);
};
