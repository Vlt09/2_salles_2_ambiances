#include "include/Renderer.hpp"

Renderer::Renderer(const Renderer &other) : _projectionMatrix(other._projectionMatrix),
                                            _viewMatrix(other._viewMatrix)
{
}

Renderer::Renderer(glm::mat4 &projMatrix, glm::mat4 &viewMatrix) : _projectionMatrix(projMatrix),
                                                                   _viewMatrix(viewMatrix)
{
}

void Renderer::render(const Geometry &object, Room::UniformMatrix uniformMatrix, const glm::mat4 &modelMatrix)
{
    auto mv_matrix = this->_viewMatrix * object.getModelMatrix();
    // auto mv_matrix = modelMatrix;
    auto normal_matrix = glm::transpose(glm::inverse(mv_matrix));

    glUniformMatrix4fv(uniformMatrix.uMVPMatrix, 1, GL_FALSE, glm::value_ptr(this->_projectionMatrix * mv_matrix));
    glUniformMatrix4fv(uniformMatrix.uMVMatrix, 1, GL_FALSE, glm::value_ptr(mv_matrix));
    glUniformMatrix4fv(uniformMatrix.uNormalMatrix, 1, GL_FALSE, glm::value_ptr(normal_matrix));

    glBindVertexArray(object.getMeshBuffer()->vao);
    glDrawArrays(GL_TRIANGLES, 0, object.getVertexCount());
    GLenum err = glGetError();
    if (err != GL_NO_ERROR)
    {
        std::cerr << "OpenGL error: " << err << std::endl;
    }
    glBindVertexArray(0);
}