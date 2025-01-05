#include "include/Renderer.hpp"

Renderer::Renderer(const Renderer &other) : _projectionMatrix(other._projectionMatrix),
                                            _viewMatrix(other._viewMatrix)
{
}

Renderer::Renderer(glm::mat4 &projMatrix, glm::mat4 &viewMatrix) : _projectionMatrix(projMatrix),
                                                                   _viewMatrix(viewMatrix)
{
}

void Renderer::renderFirstRoom(Room &firstRoom, const glm::vec3 &cameraPos, const glm::vec3 &border)
{
    auto &box = firstRoom.getBox();
    auto &uv = firstRoom.getBoxUniformVariable();
    auto spots = firstRoom.getSpotLightsData();
    auto &tCube = firstRoom.getTwistCube();
    auto &cy = firstRoom.getCylinder();
    auto &tore = firstRoom.getTore();

    if (cameraPos.x >= border.x)
    {
        box.getProgram().use();
    }

    glUniform1i(uv.uActiveLight, 1);
    setSpotLightsUniform(firstRoom);

    glBeginTransformFeedback(GL_TRIANGLES);
    glDisable(GL_BLEND);

    glUniform1i(uv.uTexLoc, 0);
    renderObject(box.getBounds(), uv);

    glEndTransformFeedback();
    glBindTexture(GL_TEXTURE_2D, 0);

    for (int i = 0; i < Room::MAX_SPOT_LIGHT; i++)
    {
        glUniform1i(uv.uHasTexture, 0);
        glUniform1i(uv.uCurrentSpotLightIdx, i);
        glUniform3f(uv.uLightIntensity, spots[i].intensity.x, spots[i].intensity.y, spots[i].intensity.z);
        renderObject(spots[i]._spot, uv);
    }

    float twistAmount = 0.005f;

    glUniform1i(uv.uHasTexture, 1);
    glUniform1i(uv.uUsePerlinNoise, 1);

    renderObject(tCube._cube, uv);

    glUniform1i(uv.uUsePerlinNoise, 0);

    renderObject(cy, uv);
    renderObject(tore, uv);

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    cy.elasticDeformation();
    tCube._cube.applyTwist(twistAmount);

    // firstRoom.printDebugBuff();
    for (int i = 0; i < Room::MAX_SPOT_LIGHT; i++)
    {
        firstRoom.moveSpot(i);
    }
    glUniform1i(uv.uActiveLight, 0); // Desac spot light for other room
}

void Renderer::renderSecondRoom(Room &sr, const glm::vec3 &cameraPos, const glm::vec3 &border)
{
    auto &box = sr.getBox();
    auto &uv = sr.getBoxUniformVariable();
    auto size = sr.glassSize();
    auto &prog = box.getProgram();
    auto src = sr.getSecondRoom();

    auto &sortedGlass = sr.getSortedGlass();
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

        glUniform3fv(uv.uCamerapos, 1, glm::value_ptr(glm::vec3(glm::mat3(_viewMatrix) * cameraPos)));
        glUniform3fv(uv.uColor, 1, glm::value_ptr(src._matColor));
        glUniform3fv(uv.uAmbientLight, 1, glm::value_ptr(src._ambientLight));
        glUniform1f(uv.uReflectance, src._reflectance);
        glUniform1f(uv.uSpecularPower, src._specularPower);

        setLightUniforms(src);
        // src.rotateDiscoAndLight();
    }

    glUniform1i(uv.uTexLoc, 0);

    renderObject(box.getBounds(), uv);
    renderObject(src._tube, uv);
    renderObject(src._ring, uv);
    renderObject(src._disco, uv);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glUniform1i(uv.uIsGlass, 1);
    for (std::map<float, Quad *>::reverse_iterator it = sortedGlass.rbegin(); it != sortedGlass.rend(); ++it)
    {
        renderObject(*it->second, uv);
    }
    glUniform1i(uv.uIsGlass, 0);

    glBindTexture(GL_TEXTURE_2D, 0);
}

void Renderer::renderSkybox(Skybox &skybox)
{
    // auto model = glm::scale(glm::mat4(1), glm::vec3(43.f));

    // auto mv_matrix = glm::mat4(glm::mat3(_viewMatrix)) * model;
    glDepthFunc(GL_LEQUAL);
    glDepthMask(GL_FALSE);
    skybox.use(); // use shader
    glUniformMatrix4fv(skybox.getUMVPLoc(), 1, GL_FALSE, glm::value_ptr(_projectionMatrix * glm::mat4(glm::mat3(_viewMatrix))));
    glBindVertexArray(skybox.getVAO());
    // glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skybox.getCubemapTexture());
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LESS);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    glBindVertexArray(0);
}

void Renderer::setMatricesToShader(const Box::UniformVariable &uniformVariable,
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

void Renderer::setMaterialAndLightingUniforms(const Box::UniformVariable &uniformVariable,
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

void Renderer::setSpotLightUniform(const Box::UniformVariable &uniformVariable, const glm::vec3 &spotLight, float spotlightCutoff, float spotlightExponent)
{
    glUniform3fv(uniformVariable.uSpotLight, 1, glm::value_ptr(spotLight));
    glUniform1f(uniformVariable.uSpotlightCutoff, cosf(glm::radians(spotlightCutoff)));
    glUniform1f(uniformVariable.uSpotlightExponent, spotlightExponent);
}

void Renderer::setSpotLightsUniform(Room &firstRoom)
{

    auto spotLight = firstRoom.getSpotLightsData();
    auto spotLightVarLoc = firstRoom.getSpotLightsUniformVarLocData();
    auto &spotMaterials = firstRoom.getSpotLightMaterials();

    // std::cout << "cut off = " << cosf(glm::radians(spotLight[1].cutoff)) << std::endl;
    for (int i = 0; i < Room::MAX_SPOT_LIGHT; i++)
    {
        glUniform3fv(spotLightVarLoc[i].position, 1, glm::value_ptr(spotLight[i].position));
        glUniform3fv(spotLightVarLoc[i].direction, 1, glm::value_ptr(spotLight[i].direction));
        glUniform3fv(spotLightVarLoc[i].lightIntensity, 1, glm::value_ptr(spotLight[i].intensity));
        // glUniform3fv(spotLightVarLoc[i].lightPos, 1, glm::value_ptr(spotLight[i].lightPos));
        glUniform3fv(spotLightVarLoc[i].m_Kd, 1, glm::value_ptr(spotMaterials[i].m_Kd));
        glUniform3fv(spotLightVarLoc[i].m_Ks, 1, glm::value_ptr(spotMaterials[i].m_Ks));
        glUniform3fv(spotLightVarLoc[i].m_Ka, 1, glm::value_ptr(spotMaterials[i].m_Ka));

        glUniform1f(spotLightVarLoc[i].cutoff, cosf(glm::radians(spotLight[i].cutoff)));
        glUniform1f(spotLightVarLoc[i].exponent, spotLight[i].exponent);
    }
}

void Renderer::renderObject(const Geometry &geometry, const Box::UniformVariable &uniformVar)
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
        }
        glDrawArrays(GL_TRIANGLES, mesh.m_nIndexOffset, mesh.m_nIndexCount);
    }

    // Unbind VAO and VBO
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Renderer::setLightUniforms(Room::SecondRoomComposent &secondRoom)
{
    auto dirLights = secondRoom._dirLight;
    auto pointLights = secondRoom._pointLight;
    auto dirLightVarLoc = secondRoom._direcLightLoc;
    auto pointLightVarLoc = secondRoom._pointLightLoc;

    for (int i = 0; i < Room::SecondRoomComposent::MAX_DIR_LIGHT; i++)
    {
        glUniform3fv(dirLightVarLoc[i].color, 1, glm::value_ptr(dirLights[i]._color));
        glUniform3fv(dirLightVarLoc[i].direction, 1, glm::value_ptr(dirLights[i]._direction));
        glUniform1f(dirLightVarLoc[i].intensity, dirLights[i]._intensity);

        if (dirLightVarLoc[i].color == -1)
            std::cerr << "Error: Could not find Directional Light color" << std::endl;
        if (dirLightVarLoc[i].direction == -1)
            std::cerr << "Error: Could not find Directional Light direction" << std::endl;
        if (dirLightVarLoc[i].intensity == -1)
            std::cerr << "Error: Could not find Directional Light intensity" << std::endl;
    }

    for (int i = 0; i < Room::SecondRoomComposent::MAX_POINT_LIGHT; i++)
    {
        glUniform3fv(pointLightVarLoc[i].color, 1, glm::value_ptr(pointLights[i]._color));
        glUniform3fv(pointLightVarLoc[i].position, 1, glm::value_ptr(pointLights[i]._position));
        glUniform1f(pointLightVarLoc[i].intensity, pointLights[i]._intensity);

        glUniform1f(pointLightVarLoc[i].constant, pointLights[i]._constant);
        glUniform1f(pointLightVarLoc[i].linear, pointLights[i]._linear);
        glUniform1f(pointLightVarLoc[i].exponent, pointLights[i]._exponent);

        if (pointLightVarLoc[i].color == -1)
            std::cerr << "Error: Could not find Point Light color" << std::endl;
        if (pointLightVarLoc[i].position == -1)
            std::cerr << "Error: Could not find Point Light position" << std::endl;
        if (pointLightVarLoc[i].intensity == -1)
            std::cerr << "Error: Could not find Point Light intensity" << std::endl;
        if (pointLightVarLoc[i].constant == -1)
            std::cerr << "Error: Could not find Point Light constant" << std::endl;
        if (pointLightVarLoc[i].linear == -1)
            std::cerr << "Error: Could not find Point Light linear" << std::endl;
        if (pointLightVarLoc[i].exponent == -1)
            std::cerr << "Error: Could not find Point Light exponent" << std::endl;
    }
}
