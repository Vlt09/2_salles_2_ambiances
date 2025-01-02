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
    auto &tCube = firstRoom.getTwistCube();
    auto &cy = firstRoom.getCylinder();

    // glm::vec3 light_dir_world = glm::rotate(glm::mat4(1.f), glimac::getTime(), glm::vec3(0, 1, 0)) * glm::vec4(1, 1, 1, 0);
    glm::vec3 light_dir_world = glm::normalize(glm::vec3(0.0f, -1.0f, 0.0f));

    setSpotLightsUniform(firstRoom);

    box.getProgram().use();

    glBeginTransformFeedback(GL_TRIANGLES);

    glUniform1i(uv.uTexLoc, 0);
    renderObject(box.getBounds(), _shaderProgram, uv);

    glEndTransformFeedback();
    glBindTexture(GL_TEXTURE_2D, 0);

    for (int i = 0; i < 2; i++)
    {
        glUniform3f(uv.uLightIntensity, spots[i].intensity.x, spots[i].intensity.y, spots[i].intensity.z);
        renderObject(spots[i]._spot, _shaderProgram, uv);
    }

    // float twistAmount = sin(glimac::getTime()) * 0.005;
    float twistAmount = 0.0005f;

    renderObject(tCube._cube, _shaderProgram, uv);
    renderObject(cy, _shaderProgram, uv);

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    cy.elasticDeformation();
    tCube._cube.applyTwist(twistAmount);

    // firstRoom.printDebugBuff();
    firstRoom.moveSpot(1);
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
        glUniform3fv(spotLightVarLoc[i].m_Kd, 1, glm::value_ptr(firstRoom._spotMaterial[i].m_Kd));
        glUniform3fv(spotLightVarLoc[i].m_Ks, 1, glm::value_ptr(firstRoom._spotMaterial[i].m_Ks));
        glUniform3fv(spotLightVarLoc[i].m_Ka, 1, glm::value_ptr(firstRoom._spotMaterial[i].m_Ka));

        glUniform1f(spotLightVarLoc[i].cutoff, cosf(glm::radians(spotLight[i].cutoff)));
        glUniform1f(spotLightVarLoc[i].exponent, spotLight[i].exponent);
    }
}

void Renderer::renderObject(const Geometry &geometry, glimac::Program &program, const Room::UniformVariable &uniformVar)
{
    program.use();

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
        glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);

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
        std::cout << "Renderer 191 mesh = " << mesh.m_nIndexOffset << " index count = " << mesh.m_nIndexCount << std::endl;
        glDrawArrays(GL_TRIANGLES, mesh.m_nIndexOffset, mesh.m_nIndexCount);
        std::cout << "Renderer 194" << std::endl;

        glBindTexture(GL_TEXTURE_2D, 0);
    }

    // Unbind VAO and VBO
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}