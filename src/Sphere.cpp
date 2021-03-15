#include "Sphere.h"
#include <math.h>
#include <memory>


// Implements the ray-sphere intersection algorithm. Details here:
// https://www.scratchapixel.com/lessons/3d-basic-rendering/minimal-ray-tracer-rendering-simple-shapes/ray-sphere-intersection
bool Sphere::localIntersect(const Ray &ray, Iset &iset_out) 
{
    // In object space, sphere is centered at 0,0,0
    // (Given ray is assumed to be already transformed to object space)
    Vector sphere_to_ray = ray.origin - Point(0,0,0);

    double a = dot(ray.dir, ray.dir);
    double b = 2 * dot(ray.dir, sphere_to_ray);
    double c = dot(sphere_to_ray, sphere_to_ray) - 1;
    double discr = b*b - 4.0*a*c;

    double t0, t1;
    if (discr < 0) return false;
    else if (discr == 0) t0 = t1 = -0.5 * b / a;
    else {
        double q = (b > 0) ?
                    -0.5 * (b + sqrt(discr)) :
                    -0.5 * (b - sqrt(discr));
        t0 = q / a;
        t1 = c / q;
    }
    if (t0 > t1) std::swap(t0,t1);

    iset_out.insert(Intersection(t0, shared_from_this()));
    iset_out.insert(Intersection(t1, shared_from_this()));
    return true;
}

Vector Sphere::localNormalAt(const Point &obj_p, const Intersection *const ip) const
{
    return ( obj_p - Point(0,0,0) );
}
