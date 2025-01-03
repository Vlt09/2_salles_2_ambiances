#pragma once
#include <glimac/BBox.hpp>
#include <glimac/glm.hpp>

/**
 * @brief Using FreeFly Camera model
 */
class Camera
{
private:
    glm::vec3 m_FrontVector, m_Position, m_LeftVector, m_UpVector;
    glm::vec3 _cameraSize = glm::vec3(4.f, 4.f, 4.f);
    float m_fPhi, m_fTheta;

    glimac::BBox3f _bbox;

    void computeDirectionVectors()
    {
        auto radTheta = glm::radians(m_fTheta);
        auto radPhi = glm::radians(m_fPhi);
        m_FrontVector = glm::vec3(glm::cos(radTheta) * glm::sin(radPhi), glm::sin(radTheta), glm::cos(radTheta) * glm::cos(radPhi));
        m_LeftVector = glm::vec3(glm::sin(radPhi + M_PI / 2), 0, glm::cos(radPhi + M_PI / 2));
        m_UpVector = glm::cross(m_FrontVector, m_LeftVector);
    }

    void updateBoundingBox()
    {
        glm::vec3 halfSize = _cameraSize * 0.5f;
        _bbox.lower = m_Position - halfSize;
        _bbox.upper = m_Position + halfSize;
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
        updateBoundingBox();
    }

    glm::vec3 nextMoveLeft(float t)
    {
        return m_Position + (t * m_LeftVector);
    }

    glm::vec3 nextMoveUp(float t)
    {
        return m_Position + (t * m_FrontVector);
    }

    void moveUp(float t)
    {
        m_Position += t * m_FrontVector;
        updateBoundingBox();
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

    const glimac::BBox3f &getBbox()
    {
        return _bbox;
    }

    /**
     * @brief Simulates the camera's next position and checks for an intersection with another bounding box.
     *
     * This function does not update the camera's position but instead creates a temporary bounding box
     * that reflects where the camera would be after the movement. It then checks for an intersection
     * between the simulated bounding box and the given bounding box.
     *
     * @param nextPos The vector representing the simulated next position of the camera.
     * @param bbox The bounding box to check for intersection.
     * @return true If the simulated position of the camera results in an intersection.
     * @return false Otherwise.
     */
    bool willIntersect(const glm::vec3 &nextPos, const glimac::BBox3f &bbox) const
    {
        glimac::BBox3f tempBBox = _bbox;

        tempBBox.lower += (nextPos - m_Position);
        tempBBox.upper += (nextPos - m_Position);

        return glimac::conjoint(tempBBox, bbox);
    }
};
