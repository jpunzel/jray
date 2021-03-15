#include "TestShape.h"
#include "util.h"
#include <math.h>
#include <memory>


bool TestShape::localIntersect(const Ray &ray, Iset &iset_out) 
{
    saved_ray = std::make_shared<Ray>(ray);
    return true;
}

Vector TestShape::localNormalAt(const Point &obj_p, const Intersection *const ip) const
{
    return Vector(0, 0, 0);
}