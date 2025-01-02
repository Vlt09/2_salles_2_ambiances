#include "include/Renderer.hpp"

Renderer::Renderer(const Renderer &other) : _projectionMatrix(other._projectionMatrix),
                                            _viewMatrix(other._viewMatrix)
{
}

Renderer::Renderer(glm::mat4 &projMatrix, glm::mat4 &viewMatrix) : _projectionMatrix(projMatrix),
                                                                   _viewMatrix(viewMatrix)
{
}

void Renderer::renderFirstRoom(FirstRoom &firstRoom, const glm::vec3 &cameraPos, const glm::vec3 &border)
{
    auto &box = firstRoom.getBox();
    auto &uv = firstRoom.getBoxUniformVariable();
    auto spots = firstRoom.getSpotLightsData();
    auto &tCube = firstRoom.getTwistCube();
    auto &cy = firstRoom.getCylinder();

    // glm::vec3 light_dir_world = glm::rotate(glm::mat4(1.f), glimac::getTime(), glm::vec3(0, 1, 0)) * glm::vec4(1, 1, 1, 0);
    glm::vec3 light_dir_world = glm::normalize(glm::vec3(0.0f, -1.0f, 0.0f));

    setSpotLightsUniform(firstRoom);

    if (cameraPos.x >= border.x)
    {
        box.getProgram().use();
    }

    glUniform1i(uv.uActiveLight, 1);

    glBeginTransformFeedback(GL_TRIANGLES);
    glDisable(GL_BLEND);

    glUniform1i(uv.uTexLoc, 0);
    renderObject(box.getBounds(), uv);

    glEndTransformFeedback();
    glBindTexture(GL_TEXTURE_2D, 0);

    for (int i = 0; i < 2; i++)
    {
        glUniform3f(uv.uLightIntensity, spots[i].intensity.x, spots[i].intensity.y, spots[i].intensity.z);
        renderObject(spots[i]._spot, uv);
    }

    float twistAmount = 0.0005f;

    renderObject(tCube._cube, uv);
    renderObject(cy, uv);

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    cy.elasticDeformation();
    tCube._cube.applyTwist(twistAmount);

    // firstRoom.printDebugBuff();
    firstRoom.moveSpot(1);
}

void Renderer::renderSecondRoom(FirstRoom &sr, const glm::vec3 &cameraPos, const glm::vec3 &border)
{
    auto &box = sr.getBox();
    auto &uv = sr.getBoxUniformVariable();
    auto size = sr.glassSize();
    auto &prog = box.getProgram();

    auto &sortedGlass = sr.getSortedGlass();
    auto &cy = sr.getCylinder();
    Quad *glass = sr.getGlassData();

    // Sort Glass using distance of an object from the viewer's perspective
    for (unsigned int i = 0; i < size; i++)
    {
        auto glassPos = glm::vec3(glass[i].getMeshBuffer()[0]._transform * glm::vec4(glass[i].getVertexBuffer()[0].m_Position, 1.0));
        float distance = glm::length(cameraPos - glassPos);
        sortedGlass[distance] = &glass[i];
    }

    if (cameraPos.x <= border.x)
    {
        prog.use();
    }

    glUniform1i(uv.uTexLoc, 0);
    glUniform1i(uv.uActiveLight, 0);

    renderObject(box.getBounds(), uv);
    renderObject(sr.getTube(), uv);
    renderObject(sr.getRing(), uv);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    for (std::map<float, Quad *>::reverse_iterator it = sortedGlass.rbegin(); it != sortedGlass.rend(); ++it)
    {
        renderObject(*it->second, uv);
    }

    glBindTexture(GL_TEXTURE_2D, 0);
    cy.applyVortexEffect(0.003f);
}

void Renderer::renderSkybox(Skybox &skybox)
{
    auto model = glm::scale(glm::mat4(1), glm::vec3(43.f));

    auto mv_matrix = glm::mat4(glm::mat3(_viewMatrix)) * model;
    skybox.use(); // use shader
    glUniformMatrix4fv(skybox.getUMVPLoc(), 1, GL_FALSE, glm::value_ptr(_projectionMatrix * mv_matrix));
    glBindVertexArray(skybox.getVAO());
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skybox.getCubemapTexture());
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
    glDepthFunc(GL_LESS);
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
        glUniform3fv(spotLightVarLoc[i].m_Kd, 1, glm::value_ptr(firstRoom._spotMaterial[i].m_Kd));
        glUniform3fv(spotLightVarLoc[i].m_Ks, 1, glm::value_ptr(firstRoom._spotMaterial[i].m_Ks));
        glUniform3fv(spotLightVarLoc[i].m_Ka, 1, glm::value_ptr(firstRoom._spotMaterial[i].m_Ka));

        glUniform1f(spotLightVarLoc[i].cutoff, cosf(glm::radians(spotLight[i].cutoff)));
        glUniform1f(spotLightVarLoc[i].exponent, spotLight[i].exponent);
    }
}

void Renderer::renderObject(const Geometry &geometry, const Room::UniformVariable &uniformVar)
{
    auto texId = geometry.getTex();
    auto &matList = geometry.getMaterialList();
    auto &meshes = geometry.getMeshVector();

    for (const auto &mesh : meshes)
    {
        if (texId != -1)
        {
            glBindTexture(GL_TEXTURE_2D, texId);
        }
        glBindVertexArray(mesh.vao);

        glm::mat4 modelMatrix = mesh.isTransform ? mesh._transform : geometry.getModelMatrix();
        glm::mat4 mv_matrix = this->_viewMatrix * modelMatrix;
        glm::mat4 normal_matrix = glm::transpose(glm::inverse(mv_matrix));

        setMatricesToShader(uniformVar, _projectionMatrix, mv_matrix, normal_matrix, modelMatrix);
        if (mesh.m_nMaterialIndex != -1)
        {
            const auto &material = matList[mesh.m_nMaterialIndex];
            glUniform3fv(uniformVar.uKa, 1, glm::value_ptr(material.m_Ka));
            glUniform3fv(uniformVar.uKd, 1, glm::value_ptr(material.m_Kd));
            glUniform3fv(uniformVar.uKs, 1, glm::value_ptr(material.m_Ks));
            glUniform1f(uniformVar.uShininess, material.m_Shininess);
            // if (material.m_pKaMap)
            // {
            //     GLuint texKaLoc = glGetUniformLocation(program.getGLId(), "uMaterial.KaMap");
            //     glUniform1i(texKaLoc, 0);
            //     glActiveTexture(GL_TEXTURE0);
            //     glBindTexture(GL_TEXTURE_2D, material.m_pKaMap->getTextureID());
            // }

            // if (material.m_pKdMap)
            // {
            //     GLuint texKdLoc = glGetUniformLocation(program.getGLId(), "uMaterial.KdMap");
            //     glUniform1i(texKdLoc, 1); // Texture unit 1
            //     glActiveTexture(GL_TEXTURE1);
            //     glBindTexture(GL_TEXTURE_2D, material.m_pKdMap->getTextureID());
            // }

            // if (material.m_pKsMap)
            // {
            //     GLuint texKsLoc = glGetUniformLocation(program.getGLId(), "uMaterial.KsMap");
            //     glUniform1i(texKsLoc, 2); // Texture unit 2
            //     glActiveTexture(GL_TEXTURE2);
            //     glBindTexture(GL_TEXTURE_2D, material.m_pKsMap->getTextureID());
            // }

            // if (material.m_pNormalMap)
            // {
            //     GLuint texNormalLoc = glGetUniformLocation(program.getGLId(), "uMaterial.NormalMap");
            //     glUniform1i(texNormalLoc, 3); // Texture unit 3
            //     glActiveTexture(GL_TEXTURE3);
            //     glBindTexture(GL_TEXTURE_2D, material.m_pNormalMap->getTextureID());
            // }
        }
        glDrawArrays(GL_TRIANGLES, mesh.m_nIndexOffset, mesh.m_nIndexCount);
    }

    // Unbind VAO and VBO
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}