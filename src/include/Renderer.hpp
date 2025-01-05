#pragma once
#include "Geometry.hpp"
#include "Scene.hpp"
#include "Box.hpp"
#include "Room.hpp"
#include "Skybox.hpp"
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

    void renderFirstRoom(Room &firstRoom, const glm::vec3 &cameraPos, const glm::vec3 &border);

    void renderSecondRoom(Room &sr, const glm::vec3 &cameraPos, const glm::vec3 &border);

    void renderSkybox(Skybox &skybox);

    void setMaterialAndLightingUniforms(const Box::UniformVariable &uniformVariable,
                                        const glm::vec3 &light_dir_vs,
                                        const glm::vec3 &light_pos_vs,
                                        const glm::vec3 &lightIntensity,
                                        const Geometry::Material &mat);

    void setMatricesToShader(const Box::UniformVariable &uniformVariable,
                             const glm::mat4 &projectionMatrix,
                             const glm::mat4 &mvMatrix,
                             const glm::mat4 &normalMatrix,
                             const glm::mat4 &modelMatrix);

    void renderScene(const Scene &scene);

    void setViewMatrix(glm::mat4 viewMatrix)
    {
        _viewMatrix = viewMatrix;
    }

    void setSpotLightUniform(const Box::UniformVariable &uniformVariable, const glm::vec3 &spotLight, float spotlightCutoff, float spotlightExponent);

    void setSpotLightsUniform(Room &firstRoom);

    void setLightUniforms(Room::SecondRoomComposent &secondRoom);

    void renderObject(const Geometry &geometry, const Box::UniformVariable &uniformVar);
};
