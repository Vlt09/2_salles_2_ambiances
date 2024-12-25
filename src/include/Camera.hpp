#pragma once
#include <glimac/glm.hpp>

/**
 * @brief Using FreeFly Camera model
 */
class Camera
{
private:
    glm::vec3 m_FrontVector, m_Position, m_LeftVector, m_UpVector;
    float m_fPhi, m_fTheta;

    void computeDirectionVectors()
    {
        auto radTheta = glm::radians(m_fTheta);
        auto radPhi = glm::radians(m_fPhi);
        m_FrontVector = glm::vec3(glm::cos(radTheta) * glm::sin(radPhi), glm::sin(radTheta), glm::cos(radTheta) * glm::cos(radPhi));
        m_LeftVector = glm::vec3(glm::sin(radPhi + M_PI / 2), 0, glm::cos(radPhi + M_PI / 2));
        m_UpVector = glm::cross(m_FrontVector, m_LeftVector);
    }

public:
    Camera()
        : m_fPhi{M_PI}, m_fTheta{0.}
    {
        computeDirectionVectors();
    }

    void moveLeft(float t)
    {
        m_Position += t * m_LeftVector;
    }

    void moveUp(float t)
    {
        m_Position += t * m_FrontVector;
    }

    void rotateLeft(float degrees)
    {
        m_fPhi -= glm::radians(degrees) * 5;
        computeDirectionVectors();
    }

    void rotateUp(float degrees)
    {
        m_fTheta += glm::radians(degrees) * 5;
        if (m_fTheta > 89.0f)
            m_fTheta = 89.0f;
        if (m_fTheta < -89.0f)
            m_fTheta = -89.0f;
        computeDirectionVectors();
    }

    glm::mat4 getViewMatrix() const
    {
        return glm::lookAt(m_Position, m_Position + m_FrontVector, normalize(m_UpVector));
    }

    const glm::vec3 cameraPosition() const
    {
        return m_Position;
    }
};
