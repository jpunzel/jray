#include "Cylinder.h"
#include <math.h>
#include "util.h"

bool Cylinder::localIntersect(const Ray &ray, Iset &iset_out) 
{
    bool hit = false;
    double a = (ray.dir.x()*ray.dir.x()) + (ray.dir.z()*ray.dir.z());
    if (a < EPSILON && a > -EPSILON) { // ray is approx parallel to the y axis so
                                       // we either hit both the end caps or we don't.
        return intersect_caps(ray, iset_out);
    }

    double b = (2 * ray.origin.x() * ray.dir.x() ) + (2 * ray.origin.z() * ray.dir.z());
    double c = (ray.origin.x()*ray.origin.x()) + (ray.origin.z()*ray.origin.z()) - 1;
    double discr = b*b - 4*a*c;

    if ( discr < 0 ) {
        return false;
    }

    double t0 = (-b - sqrt(discr)) / (2*a);
    double t1 = (-b + sqrt(discr)) / (2*a);

    if (t0 > t1) {
        std::swap(t0, t1);
    }

    double y0 = ray.origin.y() + t0 * ray.dir.y();
    if ( min_y < y0 && y0 < max_y ) {
        hit = true;
        iset_out.insert(Intersection(t0, shared_from_this()));
    }
    double y1 = ray.origin.y() + t1 * ray.dir.y();
    if ( min_y < y1 && y1 < max_y ) {
        hit = true;
        iset_out.insert(Intersection(t1, shared_from_this()));
    }

    bool caphit = intersect_caps(ray, iset_out);

    return ( hit || caphit );
}

Vector Cylinder::localNormalAt(const Point &obj_p, const Intersection *const ip) const
{
    double dist = obj_p.x()*obj_p.x() + obj_p.z()*obj_p.z();

    if ( dist < 1 && ( obj_p.y() >= (max_y - EPSILON) ) ) {
        return Vector(0,1,0);
    } else if ( dist < 1 && ( obj_p.y() <= (min_y + EPSILON) ) ) {
        return Vector(0,-1,0);
    }
    return Vector(obj_p.x(), 0, obj_p.z());
}

// Checks if intersection at t is within 1 unit from the y axis
// (the radius of our cylinder in object space).
inline bool Cylinder::check_cap(const Ray &ray, double t)
{
    double x = ray.origin.x() + t * ray.dir.x();
    double z = ray.origin.z() + t * ray.dir.z();
    return ( (x*x + z*z) <= 1 );
}

inline bool Cylinder::intersect_caps(const Ray &ray, Iset &iset_out)
{
    bool hit = false;
    // Obviously we only intersect the end caps if our cylinder is closed
    // and if we have action in the ray's y direction
    if ( !is_closed || (ray.dir.y() < EPSILON && ray.dir.y() > -EPSILON) ) {
        return false;
    }

    // Check the intersection of ray with plane at bottom of cylinder (y=min_y)
    double t = (min_y - ray.origin.y()) / ray.dir.y();
    if (check_cap(ray, t)) {
        hit = true;
        iset_out.insert(Intersection(t, shared_from_this()));
    }

    // do the same for top of cylinder (plane where y=max_y)
    t = (max_y - ray.origin.y()) / ray.dir.y();
    if (check_cap(ray, t)) {
        hit = true;
        iset_out.insert(Intersection(t, shared_from_this()));
    }
    return hit;
}