#include "include/Renderer.hpp"

Renderer::Renderer(const Renderer &other) : _projectionMatrix(other._projectionMatrix),
                                            _viewMatrix(other._viewMatrix)
{
}

Renderer::Renderer(glm::mat4 &projMatrix, glm::mat4 &viewMatrix) : _projectionMatrix(projMatrix),
                                                                   _viewMatrix(viewMatrix)
{
}

void Renderer::renderFirstRoom(FirstRoom &firstRoom)
{
    auto &box = firstRoom.getBox();
    auto &uv = firstRoom.getBoxUniformVariable();
    auto spots = firstRoom.getSpotLightsData();
    auto &torch = firstRoom.getTorch();
    auto &spotLight = firstRoom.getSpotLight(0);

    // glm::vec3 light_dir_world = glm::rotate(glm::mat4(1.f), glimac::getTime(), glm::vec3(0, 1, 0)) * glm::vec4(1, 1, 1, 0);
    glm::vec3 light_dir_world = glm::normalize(glm::vec3(0.0f, -1.0f, 0.0f));

    setSpotLightUniform(uv, spotLight.position, spotLight.cutoff, spotLight.exponent);
    setSpotLightsUniform(firstRoom);

    auto meshProcess = [&](const Geometry::Mesh &mesh, const glm::vec3 &lightIntensity, const Geometry::Material &mat)
    {
        glBindVertexArray(mesh.vao);

        glm::mat4 mv_matrix = this->_viewMatrix * mesh._transform;
        glm::mat4 normal_matrix = glm::transpose(glm::inverse(mv_matrix));
        glm::vec3 light_dir_vs = glm::vec3(glm::vec4(light_dir_world, 1.0));
        glm::vec3 light_pos_vs = glm::vec3(mesh._transform * glm::vec4(firstRoom.getGlobalLightPos(), 1.0));

        setMatricesToShader(uv, _projectionMatrix, mv_matrix, normal_matrix, mesh._transform);

        if (firstRoom.getLightFlag() == 1)
        {
            setMaterialAndLightingUniforms(uv, light_dir_vs, light_pos_vs, lightIntensity, mat);
        }

        glDrawArrays(GL_TRIANGLES, mesh.m_nIndexOffset, mesh.m_nIndexCount);
    };

    box.getProgram().use();

    glBindTexture(GL_TEXTURE_2D, box.getBounds().getTex());
    glUniform1i(uv.uTexLoc, 0);

    glBeginTransformFeedback(GL_TRIANGLES);
    applyToAllMeshes(box.getBounds().getMeshVector(), meshProcess, firstRoom._boxMaterial, firstRoom.getBoxLightIntensity());
    glEndTransformFeedback();
    glBindTexture(GL_TEXTURE_2D, 0);

    // glBindTexture(GL_TEXTURE_2D, glowStoneProg._glowStone.getTex());
    // glUniform1i(glowStoneProg._uniformVariable.uTexLoc, 0);

    for (int i = 0; i < 1; i++)
    {
        applyToAllMeshes(spots[i]._spot.getMeshVector(), meshProcess, firstRoom._spotMaterial, spots[i].intensity);
    }

    // applyToAllMeshes(torch.getMeshVector(), meshProcess);

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    // sun.animateSphere();
    // firstRoom.printDebugBuff();
    // exit(0);
}

template <typename Func>
void Renderer::applyToAllMeshes(const std::vector<Geometry::Mesh> &meshes, Func &&func, const Geometry::Material &mat, const glm::vec3 &lightIntensity)
{
    for (auto &mesh : meshes)
    {
        func(mesh, lightIntensity, mat);
    }
}

void Renderer::setMatricesToShader(const Room::UniformVariable &uniformVariable,
                                   const glm::mat4 &projectionMatrix,
                                   const glm::mat4 &mvMatrix,
                                   const glm::mat4 &normalMatrix,
                                   const glm::mat4 &modelMatrix)
{
    glUniformMatrix4fv(uniformVariable.uMVPMatrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix * mvMatrix));
    glUniformMatrix4fv(uniformVariable.uMVMatrix, 1, GL_FALSE, glm::value_ptr(mvMatrix));
    glUniformMatrix4fv(uniformVariable.uNormalMatrix, 1, GL_FALSE, glm::value_ptr(normalMatrix));
    glUniformMatrix4fv(uniformVariable.uModelMatrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));
}

void Renderer::setMaterialAndLightingUniforms(const Room::UniformVariable &uniformVariable,
                                              const glm::vec3 &light_dir_vs,
                                              const glm::vec3 &light_pos_vs,
                                              const glm::vec3 &lightIntensity,
                                              const Geometry::Material &mat)
{
    glUniform3fv(uniformVariable.uLightDir_vs, 1, glm::value_ptr(light_dir_vs));
    glUniform3fv(uniformVariable.uLightPos_vs, 1, glm::value_ptr(light_pos_vs));

    glUniform3f(uniformVariable.uLightIntensity, lightIntensity.x, lightIntensity.y, lightIntensity.z);

    glUniform3fv(uniformVariable.uKd, 1, glm::value_ptr(mat.m_Kd));
    glUniform3fv(uniformVariable.uKs, 1, glm::value_ptr(mat.m_Ks));
    glUniform1f(uniformVariable.uShininess, mat.m_Shininess);
}

void Renderer::setSpotLightUniform(const Room::UniformVariable &uniformVariable, const glm::vec3 &spotLight, float spotlightCutoff, float spotlightExponent)
{
    glUniform3fv(uniformVariable.uSpotLight, 1, glm::value_ptr(spotLight));
    glUniform1f(uniformVariable.uSpotlightCutoff, cosf(glm::radians(spotlightCutoff)));
    glUniform1f(uniformVariable.uSpotlightExponent, spotlightExponent);
}

void Renderer::setSpotLightsUniform(FirstRoom &firstRoom)
{

    auto spotLight = firstRoom.getSpotLightsData();
    auto spotLightVarLoc = firstRoom.getSpotLightsUniformVarLocData();

    // std::cout << "cut off = " << cosf(glm::radians(spotLight[1].cutoff)) << std::endl;
    for (int i = 0; i < 2; i++)
    {
        glUniform3fv(spotLightVarLoc[i].position, 1, glm::value_ptr(spotLight[i].position));
        glUniform3fv(spotLightVarLoc[i].direction, 1, glm::value_ptr(spotLight[i].direction));
        glUniform3fv(spotLightVarLoc[i].lightIntensity, 1, glm::value_ptr(spotLight[i].intensity));
        // glUniform3fv(spotLightVarLoc[i].lightPos, 1, glm::value_ptr(spotLight[i].lightPos));
        glUniform3fv(spotLightVarLoc[i].m_Kd, 1, glm::value_ptr(firstRoom._spotMaterial.m_Kd));
        glUniform3fv(spotLightVarLoc[i].m_Ks, 1, glm::value_ptr(firstRoom._spotMaterial.m_Ks));

        glUniform1f(spotLightVarLoc[i].cutoff, cosf(glm::radians(spotLight[i].cutoff)));
        glUniform1f(spotLightVarLoc[i].exponent, spotLight[i].exponent);
    }
}