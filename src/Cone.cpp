#include "Cone.h"
#include <math.h>
#include "util.h"


// Intersect a double-napped cone. This is mostly the same as cylinder intersection.
bool Cone::localIntersect(const Ray &ray, Iset &iset_out) 
{
    bool hit = false;
    double a = (ray.dir.x()*ray.dir.x()) - (ray.dir.y()*ray.dir.y()) + (ray.dir.z()*ray.dir.z());
    double b = (2 * ray.origin.x() * ray.dir.x() ) - (2 * ray.origin.y() * ray.dir.y()) + (2 * ray.origin.z() * ray.dir.z());
    double c = (ray.origin.x()*ray.origin.x()) - (ray.origin.y()*ray.origin.y()) + (ray.origin.z()*ray.origin.z());
    bool caphit = intersect_caps(ray, iset_out);
    //if (a < EPSILON && a > -EPSILON) { // ray is approx parallel to one of the cone halves
    if ( a == 0 ) {
        //if (b < EPSILON && b > -EPSILON) {
        if (b == 0) {
            return caphit;
        } else {
            hit = true;
            double t = -c / (2*b);
            iset_out.insert(Intersection(t, shared_from_this()));
            return true;
        }
    }

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

    return ( hit || caphit );
}

Vector Cone::localNormalAt(const Point &obj_p, const Intersection *const ip) const
{
    double dist = obj_p.x()*obj_p.x() + obj_p.z()*obj_p.z();

    if ( dist < max_y*max_y && ( obj_p.y() >= (max_y - EPSILON) ) ) {
        return Vector(0,1,0);
    } else if ( dist < min_y*min_y && ( obj_p.y() <= (min_y + EPSILON) ) ) {
        return Vector(0,-1,0);
    }

    double y = sqrt(dist);
    if ( obj_p.y() > 0 ) {
        y = -y;
    }

    return Vector(obj_p.x(), y, obj_p.z());
}

// Checks if intersection at t is within radius=|y| distance from the y axis
// (the radius of our cone in object space is equal to absolute value of y).
inline bool Cone::check_cap(const Ray &ray, double t, double y)
{
    double x = ray.origin.x() + t * ray.dir.x();
    double z = ray.origin.z() + t * ray.dir.z();
    return ( (x*x + z*z) <= abs(y*y) );
}

inline bool Cone::intersect_caps(const Ray &ray, Iset &iset_out)
{
    bool hit = false;
    // Obviously we only intersect the end caps if our cone is closed
    // and if we have action in the ray's y direction
    if ( !is_closed || (ray.dir.y() < EPSILON && ray.dir.y() > -EPSILON) ) {
        return false;
    }

    // Check the intersection of ray with plane at bottom of cone (y=min_y)
    double t = (min_y - ray.origin.y()) / ray.dir.y();
    if (check_cap(ray, t, min_y)) {
        hit = true;
        iset_out.insert(Intersection(t, shared_from_this()));
    }

    // do the same for top of cone (plane where y=max_y)
    t = (max_y - ray.origin.y()) / ray.dir.y();
    if (check_cap(ray, t, max_y)) {
        hit = true;
        iset_out.insert(Intersection(t, shared_from_this()));
    }
    return hit;
}