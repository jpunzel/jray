#include "Cube.h"
#include "util.h"
#include <math.h>
#include <memory>
#include <utility>

// Basic idea: consider a cube as 3 pairs of parallel planes
// that are one unit away from the origin in object space.
// This is an axis-aligned bounding box (AABB).
// The two intersections on a cube will be the largest minimum
// and the smallest maximum t value for each of x, y, z 
bool Cube::localIntersect(const Ray &ray, Iset &iset_out) 
{
    std::pair<double,double> xtminmax, ytminmax, ztminmax;
    double xtmin, xtmax, ytmin, ytmax, ztmin, ztmax;
    double tmin, tmax;
    xtminmax = check_axis(ray.origin.x(), ray.dir.x());
    ytminmax = check_axis(ray.origin.y(), ray.dir.y());
    ztminmax = check_axis(ray.origin.z(), ray.dir.z());

    xtmin = xtminmax.first;
    xtmax = xtminmax.second;
    ytmin = ytminmax.first;
    ytmax = ytminmax.second;
    ztmin = ztminmax.first;
    ztmax = ztminmax.second;

    tmin = std::max({xtmin, ytmin, ztmin});
    tmax = std::min({xtmax, ytmax, ztmax});

    if (tmin > tmax) { // we missed
        return false; // iset_out remains empty
    }

    iset_out.insert(Intersection(tmin, shared_from_this()));
    iset_out.insert(Intersection(tmax, shared_from_this()));
    return true;
}

inline std::pair<double,double> Cube::check_axis(double origin, double direction)
{
    // Our cube in object space is always an axially-aligned bounding box
    // with planes at +1 and -1 for each axis.
    double tmin_numerator = (-1 - origin);
    double tmax_numerator = (1 - origin);
    double tmin, tmax;

    tmin = tmin_numerator / direction;
    tmax = tmax_numerator / direction;

    if ( tmin > tmax ) {
        std::swap(tmin, tmax);
    }

    return std::make_pair(tmin, tmax);
}

//
// The below algorithm SHOULD be faster but it appears that compiler optimizations
// are smart enough to make the easier-to-understand implementation perform the same.
// Keeping it here just in case.
// 

//bool Cube::localIntersect(const Ray &ray, Iset &iset_out)
//{
//    double tmin, tmax, ytmin, ytmax, ztmin, ztmax;
//    Vector invdir = 1 / ray.dir;
//    bool sign_x = (invdir.x() >= 0);
//    bool sign_y = (invdir.y() >= 0);
//    bool sign_z = (invdir.z() >= 0);
//
//    // First get min/max of plane intersections along each axis
//    // (tmin/tmax will hold our final intersection values, but we initially use them
//    // for the x axis)
//    check_axis(sign_x, ray.origin.x(), invdir.x(), tmin, tmax);
//    check_axis(sign_y, ray.origin.y(), invdir.y(), ytmin, ytmax);
//
//    if ( (tmin > ytmax) || (ytmin > tmax) )
//        return false; // we missed. iset_out remains empty.
//
//    // Provisionally set tmin/tmax to that of the y component
//    // if they match our criteria (we want the largest minimum and smallest maximum)
//    if ( ytmin > tmin )
//        tmin = ytmin;
//    if ( ytmax < tmax )
//        tmax = ytmax;
//
//    check_axis(sign_z, ray.origin.z(), invdir.z(), ztmin, ztmax);
//
//    if ( (tmin > ztmax) || (ztmin > tmax) )
//        return false; // we missed. iset_out remains empty.
//    
//    if ( ztmin > tmin )
//        tmin = ztmin;
//    if ( ztmax < tmax )
//        tmax = ztmax;
//
//    iset_out.insert(Intersection(tmin, shared_from_this()));
//    iset_out.insert(Intersection(tmax, shared_from_this()));
//    return true;
//}

//inline void Cube::check_axis(bool sign, double origin, double invdir, double &tmin_out, double &tmax_out)
//{
//    // Our cube in object space is always an axially-aligned bounding box
//    // with planes at +1 and -1 for each axis. To avoid having to call swap()
//    // after min/max are determined, we first check the sign of the ray component.
//    if (sign) {
//        tmin_out = (-1 - origin) * invdir;
//        tmax_out = (1 - origin) * invdir;
//    } else {
//        tmin_out = (1 - origin) * invdir;
//        tmax_out = (-1 - origin) * invdir;
//    }
//}


Vector Cube::localNormalAt(const Point &obj_p, const Intersection *const ip) const
{
    // This is super easy, since our cube is always an AABB we just figure out
    // which component of the point is at ~1.0, and our normal is perpendicular
    // to the associated plane
    double abs_x = abs(obj_p.x());
    double abs_y = abs(obj_p.y());
    double abs_z = abs(obj_p.z());
    double maxc = std::max({ abs_x, abs_y, abs_z });
    
    if ( maxc == abs_x ) {
        return Vector(obj_p.x(), 0, 0);
    } else if ( maxc == abs_y ) {
        return Vector(0, obj_p.y(), 0);
    }
    return Vector(0, 0, obj_p.z());
}