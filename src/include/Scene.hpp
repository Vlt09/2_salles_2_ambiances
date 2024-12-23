#pragma once
#include <list>
#include <glimac/Geometry.hpp>

class Scene
{
private:
    std::list<glimac::Geometry> _objects;

public:
    Scene(/* args */);
    ~Scene();
};