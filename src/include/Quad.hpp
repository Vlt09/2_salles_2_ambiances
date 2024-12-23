#pragma once
#include <GLFW/glfw3.h>
#include "include/Geometry.hpp"

class Quad : glimac::Geometry
{
private:
    GLsizei m_nVertexCount;

public:
    Quad(/* args */);
    ~Quad();
};
