#pragma once
#include <list>
#include "include/Geometry.hpp"

class Scene
{
private:
    std::list<glimac::Geometry> _objects;

public:
    Scene(/* args */);
    ~Scene();
};