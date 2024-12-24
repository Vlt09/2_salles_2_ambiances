#include "include/Renderer.hpp"

void Renderer::render(const Geometry &object, Room::UniformMatrix uniformMatrix, const glm::mat4 &modelMatrix)
{
    auto mv_matrix = this->_viewMatrix * modelMatrix;
    auto normal_matrix = glm::transpose(glm::inverse(mv_matrix));

    glUniformMatrix4fv(uniformMatrix.uMVMatrix, 1, GL_FALSE, glm::value_ptr(this->_projectionMatrix * mv_matrix));
    glUniformMatrix4fv(uniformMatrix.uMVMatrix, 1, GL_FALSE, glm::value_ptr(mv_matrix));
    glUniformMatrix4fv(uniformMatrix.uNormalMatrix, 1, GL_FALSE, glm::value_ptr(normal_matrix));
}