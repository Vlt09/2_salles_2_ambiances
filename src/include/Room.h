#pragma once
#include <list>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Geometry.hpp"
#include <glimac/Program.hpp>

class Room
{
public:
    struct UniformMatrix
    {
        GLint uMVPMatrix;
        GLint uMVMatrix;
        GLint uNormalMatrix;
    };

private:
    std::list<Geometry> _objects;
    glimac::Program _mProgram;
    UniformMatrix uniformMatrix;

    Room(/* args */);
    ~Room();
};
