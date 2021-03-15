#include "Triangle.h"

bool Triangle::localIntersect(const Ray &ray, Iset &iset_out)
{
    Vector dir_cross_e2 = cross(ray.dir, e2);
    double det = dot(e1, dir_cross_e2);
    if ( abs(det) < EPSILON ) { // ray is parallel
        return false;
    }

    double f = 1.0 / det;
    Vector p1_to_origin = ray.origin - p1;
    double u = f * dot(p1_to_origin, dir_cross_e2);
    if ( u < 0 || u > 1 ) { // ray misses p1-p3 edge
        return false;
    }

    Vector origin_cross_e1 = cross(p1_to_origin, e1);
    double v = f * dot(ray.dir, origin_cross_e1);
    if ( v < 0 || (u+v) > 1 ) { // ray misses p2-p3 and p1-p2 edge
        return false;
    }

    // ray hits
    double t = f * dot(e2, origin_cross_e1);

    // Uniquely for triangles, we construct the Intersection with u and v as well.
    // This will be used for normal interpolation on smooth triangles.
    iset_out.insert(Intersection(t, u, v, shared_from_this()));
    return true;
}

Vector Triangle::localNormalAt(const Point &obj_p, const Intersection *const ip) const
{
    if (!isSmooth)
        return n1;
    
    // normal interpolation
    return (n2 * ip->u + n3 * ip->v + n1 * (1 - ip->u - ip->v) );

}