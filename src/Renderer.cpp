#include "include/Renderer.hpp"

Renderer::Renderer(const Renderer &other) : _projectionMatrix(other._projectionMatrix),
                                            _viewMatrix(other._viewMatrix)
{
}

Renderer::Renderer(glm::mat4 &projMatrix, glm::mat4 &viewMatrix) : _projectionMatrix(projMatrix),
                                                                   _viewMatrix(viewMatrix)
{
}

void Renderer::render(const Geometry &object, Room::UniformMatrix uniformMatrix, GLuint tex)
{
    if (uniformMatrix.uTexLoc == -1)
    {
        std::cerr << "L'uniforme de texture n'est pas lié correctement." << std::endl;
    }
    // auto mv_matrix = this->_viewMatrix * object.getModelMatrix();
    // // auto mv_matrix = modelMatrix;
    // auto normal_matrix = glm::transpose(glm::inverse(mv_matrix));

    // glUniformMatrix4fv(uniformMatrix.uMVPMatrix, 1, GL_FALSE, glm::value_ptr(this->_projectionMatrix * mv_matrix));
    // glUniformMatrix4fv(uniformMatrix.uMVMatrix, 1, GL_FALSE, glm::value_ptr(mv_matrix));
    // glUniformMatrix4fv(uniformMatrix.uNormalMatrix, 1, GL_FALSE, glm::value_ptr(normal_matrix));
    glBindTexture(GL_TEXTURE_2D, tex); // earth
    glUniform1i(uniformMatrix.uTexLoc, 0);

    auto meshBuff = object.getMeshBuffer();
    auto meshCount = object.getMeshCount();
    for (size_t i = 0; i < meshCount; i++)
    {
        auto &currentMesh = meshBuff[i];
        glBindVertexArray(currentMesh.vao);

        if (currentMesh.isTransform)
        {
            auto mv_matrix = this->_viewMatrix * currentMesh._transform;
            auto normal_matrix = glm::transpose(glm::inverse(mv_matrix));

            glUniformMatrix4fv(uniformMatrix.uMVPMatrix, 1, GL_FALSE, glm::value_ptr(this->_projectionMatrix * mv_matrix));
            glUniformMatrix4fv(uniformMatrix.uMVMatrix, 1, GL_FALSE, glm::value_ptr(mv_matrix));
            glUniformMatrix4fv(uniformMatrix.uNormalMatrix, 1, GL_FALSE, glm::value_ptr(normal_matrix));
        }

        // glBindVertexArray(currentMesh.vao);
        // glBindTexture(GL_TEXTURE_2D, tex);
        glDrawArrays(GL_TRIANGLES, currentMesh.m_nIndexOffset, currentMesh.m_nIndexCount);
    }
    glBindVertexArray(0);

    glBindTexture(GL_TEXTURE_2D, 0);
}
