#pragma once

#include "Point.h"
#include "Vector.h"
#include "Matrix.h"
#include <memory>
#include <utility>
#include <set>

// Forward declarations
class Shape;
class Intersection;

// Icomps is a simple struct for storing precomputed intersection computations.
struct Icomps {
    double t;
    std::shared_ptr<Shape> obj;
    Point point;
    Point over_point; // Slightly offset by EPSILON in the direction of the normal vector.
                      // over_point is used in shading to prevent shadow acne
    Point under_point; // Slightly offset away from the normal vector.
                       // under_point is used for refractions.
    Vector eyev;
    Vector normalv;
    Vector reflectv;
    bool inside; // true if intersection is inside shape pointed to by obj
    double rindex_from; // refractive index of previous object a.k.a n1
    double rindex_to; // refractive index of next object a.k.a n2

    double schlick() const;
};

class Ray {
public:
    Ray(Point p, Vector d): origin(p), dir(d) { }
    inline Point pos(double t) const { return ( origin + dir*t ); }
    inline Ray transform(const Matrix &M) const { return Ray(M * origin, M * dir); }
    Point origin;
    Vector dir;
};

// Iset: a simple std::multiset used to store all intersections of a Ray.
// We use multiset instead of unique set because in certain cases 
// (i.e, a hit tangent to the object) our intersect function will return 
// two identical intersections and this fact will help us distinguish those.

typedef std::multiset<Intersection> Iset;

class Intersection {
public:
    Intersection() : t(0.0), obj(nullptr) { }
    Intersection(double t, const std::shared_ptr<Shape> &object) : t(t), obj(object) { }
    Intersection(double t, double u, double v, const std::shared_ptr<Shape> &object) : t(t), u(u), v(v), obj(object) { }
    friend bool operator<(const Intersection &i1, const Intersection &i2) { return i1.t < i2.t; }
    friend bool operator==(const Intersection &i1, const Intersection &i2)
        { return (i1.t == i2.t && i1.obj == i2.obj); }
    friend bool operator!=(const Intersection &i1, const Intersection &i2) { return (i1 != i2); }
    inline bool isEmpty() const { return obj == nullptr; }

    Icomps prepComps(const Ray &r);
    Icomps prepComps(const Ray &r, const Iset &xs);

    double t;
    double u,v; // These values are only used for normal interpolation on smooth triangles.
    std::shared_ptr<Shape> obj;
};

inline Intersection hit(const Iset &iset)
{
    // return first non-negative Intersection (easy since std::multiset is sorted
    // and we defined comparison operator for Intersection to use t value)
    auto itr = iset.begin();
    while (itr != iset.end()) {
        if (itr->t > 0) {
            return (*itr);
        }
        ++itr;
    }
    // if no match, return empty Intersection
    Intersection i = Intersection();
    return i; 
}