#pragma once
#include "maths.vector.hpp"

class Rotation
{
public:
    Rotation() : deg(0), axis(0.0f, 0.0f, 0.0f) {}
    Rotation(float d, Vector3& ax) : deg(d), axis(ax) {}
    float deg;
    Vector3 axis;
};
