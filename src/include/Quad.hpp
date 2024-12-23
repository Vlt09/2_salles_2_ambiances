#pragma once
#include <GLFW/glfw3.h>
#include <glimac/Geometry.hpp>

class Quad : glimac::Geometry
{
private:
    GLsizei m_nVertexCount;

public:
    Quad(/* args */);
    ~Quad();
};
