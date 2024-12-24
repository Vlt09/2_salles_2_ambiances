#pragma once
#include <list>
#include "Geometry.hpp"

class Scene
{
private:
    std::list<Geometry> _objects;

public:
    Scene(/* args */);
    ~Scene();
};