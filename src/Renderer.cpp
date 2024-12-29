#include "include/Renderer.hpp"

Renderer::Renderer(const Renderer &other) : _projectionMatrix(other._projectionMatrix),
                                            _viewMatrix(other._viewMatrix)
{
}

Renderer::Renderer(glm::mat4 &projMatrix, glm::mat4 &viewMatrix) : _projectionMatrix(projMatrix),
                                                                   _viewMatrix(viewMatrix)
{
}

void renderMultipleMesh(int meshCount, const Geometry::Mesh *meshBuff, glm::vec3 &light_dir_world,
                        glm::mat4 &projMat, glm::mat4 &viewMat, Room::UniformVariable &uniformVariable, const std::vector<Geometry::Material> &matList)
{
    for (size_t i = 0; i < meshCount; i++)
    {
        auto &currentMesh = meshBuff[i];
        glBindVertexArray(currentMesh.vao);

        // Check if Mesh has local transform
        if (currentMesh.isTransform)
        {
            auto mv_matrix = viewMat * currentMesh._transform;
            glm::vec3 light_dir_vs = glm::vec3(mv_matrix * glm::vec4(light_dir_world, 0.0));

            auto normal_matrix = glm::transpose(glm::inverse(mv_matrix));

            glUniformMatrix4fv(uniformVariable.uMVPMatrix, 1, GL_FALSE, glm::value_ptr(projMat * mv_matrix));
            glUniformMatrix4fv(uniformVariable.uMVMatrix, 1, GL_FALSE, glm::value_ptr(mv_matrix));
            glUniformMatrix4fv(uniformVariable.uNormalMatrix, 1, GL_FALSE, glm::value_ptr(normal_matrix));

            // Light parameter
            glUniform3fv(uniformVariable.uLightDir_vs, 1, glm::value_ptr(light_dir_vs));
            glUniform3f(uniformVariable.uLightIntensity, 1.0f, 1.0f, 1.0f);

            if (currentMesh.m_nMaterialIndex != -1)
            {
                // Get the material
                const Geometry::Material &material = matList[currentMesh.m_nMaterialIndex];

                glUniform3fv(uniformVariable.uKd, 1, glm::value_ptr(material.m_Kd)); // Diffuse color
                glUniform3fv(uniformVariable.uKs, 1, glm::value_ptr(material.m_Ks)); // Specular color
                glUniform1f(uniformVariable.uShininess, material.m_Shininess);       // Shininess coefficient
            }
            else
            {
                glUniform3f(uniformVariable.uKd, 0.4f, 0.3f, 0.2f);
                glUniform3f(uniformVariable.uKs, 0.05f, 0.05f, 0.05f);
                glUniform1f(uniformVariable.uShininess, 2.0f);
            }
        }

        // glBindVertexArray(currentMesh.vao);
        // glBindTexture(GL_TEXTURE_2D, tex);
        glDrawArrays(GL_TRIANGLES, currentMesh.m_nIndexOffset, currentMesh.m_nIndexCount);
    }
}

void Renderer::render(const Geometry &object, Room::UniformVariable uniformVariable)
{
    if (uniformVariable.uTexLoc == -1)
    {
        std::cerr << "L'uniforme de texture n'est pas lié correctement." << std::endl;
    }

    glm::vec3 light_dir_world = glm::rotate(glm::mat4(1.f), glimac::getTime(), glm::vec3(0, 1, 0)) * glm::vec4(1, 1, 1, 0);

    glBindTexture(GL_TEXTURE_2D, object.getTex());
    glUniform1i(uniformVariable.uTexLoc, 0);

    auto meshBuff = object.getMeshBuffer();
    auto meshCount = object.getMeshCount();
    const auto &matList = object.getMaterialList();

    // Check if mesh's object has local transform
    if (meshCount > 1 && meshBuff->isTransform == true)
    {
        renderMultipleMesh(meshCount, meshBuff, light_dir_world, this->_projectionMatrix, this->_viewMatrix, uniformVariable, matList);
    }
    else
    {
        auto mv_matrix = this->_viewMatrix * object.getModelMatrix();
        glm::vec3 light_dir_vs = glm::vec3(mv_matrix * glm::vec4(light_dir_world, 0.0));

        auto normal_matrix = glm::transpose(glm::inverse(mv_matrix));

        glBindVertexArray(meshBuff[0].vao);

        glUniformMatrix4fv(uniformVariable.uMVPMatrix, 1, GL_FALSE, glm::value_ptr(this->_projectionMatrix * mv_matrix));
        glUniformMatrix4fv(uniformVariable.uMVMatrix, 1, GL_FALSE, glm::value_ptr(mv_matrix));
        glUniformMatrix4fv(uniformVariable.uNormalMatrix, 1, GL_FALSE, glm::value_ptr(normal_matrix));

        // Light parameter
        glUniform3fv(uniformVariable.uLightDir_vs, 1, glm::value_ptr(light_dir_vs));
        glUniform3f(uniformVariable.uLightIntensity, 1.0f, 1.0f, 1.0f);

        glUniform3f(uniformVariable.uKd, 0.8f, 0.8f, 0.8f);
        glUniform3f(uniformVariable.uKs, 0.5f, 0.5f, 0.5f);
        glUniform1f(uniformVariable.uShininess, 32.0f);

        if (meshBuff[0].m_nMaterialIndex != -1)
        {
            auto material = object.getMaterialList()[0];
            glUniform3fv(uniformVariable.uKd, 1, glm::value_ptr(material.m_Kd)); // Diffuse color
            glUniform3fv(uniformVariable.uKs, 1, glm::value_ptr(material.m_Ks)); // Specular color
            glUniform1f(uniformVariable.uShininess, material.m_Shininess);       // Shininess coefficient
        }

        glDrawArrays(GL_TRIANGLES, 0, object.getVertexCount());
    }

    glBindVertexArray(0);

    glBindTexture(GL_TEXTURE_2D, 0);
}

void Renderer::renderFirstRoom(FirstRoom &firstRoom)
{
    auto &box = firstRoom.getBox();
    auto &uv = firstRoom.getBoxUniformVariable();
    auto &sun = firstRoom.getSpot();
    auto &torch = firstRoom.getTorch();
    auto &glowStoneProg = firstRoom.getGlowStoneProg();
    auto &spotLight = firstRoom.getSpotLight();

    // glm::vec3 light_dir_world = glm::rotate(glm::mat4(1.f), glimac::getTime(), glm::vec3(0, 1, 0)) * glm::vec4(1, 1, 1, 0);
    glm::vec3 light_dir_world = glm::normalize(glm::vec3(1.0f, -1.0f, 1.0f));

    setSpotLightUniform(uv, spotLight.position, spotLight.cutoff, spotLight.exponent);

    auto meshProcess = [&](const Geometry::Mesh &mesh)
    {
        glBindVertexArray(mesh.vao);

        glm::mat4 mv_matrix = this->_viewMatrix * mesh._transform;
        glm::mat4 normal_matrix = glm::transpose(glm::inverse(mv_matrix));
        glm::vec3 light_dir_vs = glm::vec3(glm::vec4(light_dir_world, 1.0));
        glm::vec3 light_pos_vs = glm::vec3(this->_viewMatrix * glm::vec4(firstRoom.getLightPos(), 1.0));

        setMatricesToShader(uv, _projectionMatrix, mv_matrix, normal_matrix);

        if (firstRoom.getLightFlag() == 1)
        {
            setMaterialAndLightingUniforms(uv, light_dir_vs, light_pos_vs, firstRoom._boxMaterial.m_Kd, firstRoom._boxMaterial.m_Ks,
                                           light_pos_vs, firstRoom._spotMaterial.m_Shininess);
        }

        glDrawArrays(GL_TRIANGLES, mesh.m_nIndexOffset, mesh.m_nIndexCount);
    };

    box.getProgram().use();

    glBindTexture(GL_TEXTURE_2D, box.getBounds().getTex());
    glUniform1i(uv.uTexLoc, 0);

    glBeginTransformFeedback(GL_TRIANGLES);
    applyToAllMeshes(box.getBounds().getMeshVector(), meshProcess);
    glEndTransformFeedback();
    glBindTexture(GL_TEXTURE_2D, 0);

    // glBindTexture(GL_TEXTURE_2D, glowStoneProg._glowStone.getTex());
    // glUniform1i(glowStoneProg._uniformVariable.uTexLoc, 0);

    applyToAllMeshes(sun.getMeshVector(), meshProcess);

    // applyToAllMeshes(torch.getMeshVector(), meshProcess);

    // applyToAllMeshes(glowStoneProg._glowStone.getMeshVector(), meshProcess);

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    // sun.animateSphere();
}

void Renderer::render(const Geometry &object, Geometry::Material material, Room::UniformVariable uniformVariable)
{
    if (uniformVariable.uTexLoc == -1)
    {
        std::cerr << "L'uniforme de texture n'est pas lié correctement." << std::endl;
    }

    glm::vec3 light_dir_world = glm::rotate(glm::mat4(1.f), glimac::getTime(), glm::vec3(0, 1, 0)) * glm::vec4(1, 1, 1, 0);

    glBindTexture(GL_TEXTURE_2D, object.getTex());
    glUniform1i(uniformVariable.uTexLoc, 0);

    glUniform3fv(uniformVariable.uKd, 1, glm::value_ptr(material.m_Kd));
    glUniform3fv(uniformVariable.uKs, 1, glm::value_ptr(material.m_Ks));
    glUniform1f(uniformVariable.uShininess, material.m_Shininess);

    auto meshBuff = object.getMeshBuffer();
    auto meshCount = object.getMeshCount();
    for (size_t i = 0; i < meshCount; i++)
    {
        auto &currentMesh = meshBuff[i];
        glBindVertexArray(currentMesh.vao);

        // Check if Mesh has local transform
        if (currentMesh.isTransform)
        {
            auto mv_matrix = this->_viewMatrix * currentMesh._transform;
            glm::vec3 light_dir_vs = glm::vec3(mv_matrix * glm::vec4(light_dir_world, 0.0));

            auto normal_matrix = glm::transpose(glm::inverse(mv_matrix));

            glUniformMatrix4fv(uniformVariable.uMVPMatrix, 1, GL_FALSE, glm::value_ptr(this->_projectionMatrix * mv_matrix));
            glUniformMatrix4fv(uniformVariable.uMVMatrix, 1, GL_FALSE, glm::value_ptr(mv_matrix));
            glUniformMatrix4fv(uniformVariable.uNormalMatrix, 1, GL_FALSE, glm::value_ptr(normal_matrix));

            // Light parameter
            glUniform3fv(uniformVariable.uLightDir_vs, 1, glm::value_ptr(light_dir_vs));
            glUniform3f(uniformVariable.uLightIntensity, 1.0f, 1.0f, 1.0f);
        }

        // glBindVertexArray(currentMesh.vao);
        // glBindTexture(GL_TEXTURE_2D, tex);
        glDrawArrays(GL_TRIANGLES, currentMesh.m_nIndexOffset, currentMesh.m_nIndexCount);
    }
    glBindVertexArray(0);

    glBindTexture(GL_TEXTURE_2D, 0);
}

template <typename Func>
void Renderer::applyToAllMeshes(const std::vector<Geometry::Mesh> &meshes, Func &&func)
{
    for (auto &mesh : meshes)
    {
        func(mesh);
    }
}

void Renderer::setMatricesToShader(const Room::UniformVariable &uniformVariable,
                                   const glm::mat4 &projectionMatrix,
                                   const glm::mat4 &mvMatrix,
                                   const glm::mat4 &normalMatrix)
{
    glUniformMatrix4fv(uniformVariable.uMVPMatrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix * mvMatrix));
    glUniformMatrix4fv(uniformVariable.uMVMatrix, 1, GL_FALSE, glm::value_ptr(mvMatrix));
    glUniformMatrix4fv(uniformVariable.uNormalMatrix, 1, GL_FALSE, glm::value_ptr(normalMatrix));
}

void Renderer::setMaterialAndLightingUniforms(const Room::UniformVariable &uniformVariable,
                                              const glm::vec3 &light_dir_vs,
                                              const glm::vec3 &light_pos_vs,
                                              const glm::vec3 &lightIntensity,
                                              const glm::vec3 &kd,
                                              const glm::vec3 &ks,
                                              float shininess)
{
    glUniform3fv(uniformVariable.uLightDir_vs, 1, glm::value_ptr(light_dir_vs));
    glUniform3fv(uniformVariable.uLightPos_vs, 1, glm::value_ptr(light_pos_vs));

    glUniform3f(uniformVariable.uLightIntensity, lightIntensity.x, lightIntensity.y, lightIntensity.z);

    glUniform3fv(uniformVariable.uKd, 1, glm::value_ptr(kd));
    glUniform3fv(uniformVariable.uKs, 1, glm::value_ptr(ks));
    glUniform1f(uniformVariable.uShininess, shininess);
}

void Renderer::setSpotLightUniform(const Room::UniformVariable &uniformVariable, const glm::vec3 &spotLight, float spotlightCutoff, float spotlightExponent)
{
    glUniform3fv(uniformVariable.uSpotLight, 1, glm::value_ptr(spotLight));
    glUniform1f(uniformVariable.uSpotlightCutoff, cosf(spotlightCutoff));
    glUniform1f(uniformVariable.uSpotlightExponent, spotlightExponent);
}
