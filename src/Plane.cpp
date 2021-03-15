#include "Plane.h"
#include "util.h"
#include <math.h>
#include <memory>


// Super-easy intersection formula. We intersect at y=0 in object space.
bool Plane::localIntersect(const Ray &ray, Iset &iset_out) 
{
    // if ray y component is zero (or close enough) we're coplanar
    if (abs(ray.dir.y()) < EPSILON) {
        return false;
    }
    double t = -ray.origin.y() / ray.dir.y();
    iset_out.insert(Intersection(t, shared_from_this()));
    return true;
}

Vector Plane::localNormalAt(const Point &obj_p, const Intersection *const ip) const
{
    // probably the simplest normal function.
    // Since our plane in object space is the x,z plane, our normal vector
    // is always the basis vector in the y direction.
    return Vector(0, 1, 0);
}