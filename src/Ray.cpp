#include "Ray.h"
#include "Shape.h"
#include "util.h"
#include <algorithm>
#include <vector>
#include <cmath>

double Icomps::schlick() const
{
    double cos = dot(eyev, normalv);

    // Check for total internal reflection
    if (rindex_from > rindex_to) {
        double n = rindex_from / rindex_to;
        double sin2t = n*n * (1.0 - cos*cos);
        if ( sin2t > 1.0 ) {
            return 1.0;
        }
        // Using trig identity
        double cos_t = sqrt(1.0 - sin2t);
        // Replace cosine only when rindex_from > rindex_to
        cos = cos_t;
    }
    double x = ((rindex_from - rindex_to) / (rindex_from + rindex_to));
    double r0 = x*x;
    return r0 + (1 - r0) * pow( (1-cos) , 5);
}

Icomps Intersection::prepComps(const Ray &r)
{
    Icomps ret;
    // Include data members for convenience
    ret.t = t;
    ret.obj = obj;

    ret.point = r.pos(t);
    ret.eyev = -r.dir;
    ret.normalv = obj->normalAt(ret.point, this);
    ret.reflectv = reflect(r.dir, ret.normalv);
    
    // We know we're inside the shape given by obj if its normal vector
    // points in roughly the opposite direction as our eye vector
    if ( dot(ret.normalv, ret.eyev)  < 0) {
        ret.inside = true;
        ret.normalv = -ret.normalv;
    } else {
        ret.inside = false;
    }
    // Most of the time we use over_point which bumps our intersection
    // just a tiny bit in the direction of the normal vector.
    // (This prevents shadow acne)
    ret.over_point = ret.point + ret.normalv * EPSILON;
    // Similarly, under_point is used when we do refractions
    ret.under_point = ret.point - ret.normalv * EPSILON;

    return ret;
}

// Overloaded version that takes the set of all Intersections to do
// refraction calculations. This is the "main" version called by colorAt().
Icomps Intersection::prepComps(const Ray &r, const Iset &xs)
{
    Icomps ret = prepComps(r); // Single-argument version does everything else first

    std::vector<std::shared_ptr<Shape>> containing_shapes;

    // Loop through (note: SORTED) set of all intersections. We want to keep track of
    // all containing shapes that the ray passed through to get to *this* intersection.
    for ( auto i : xs ) {

        // If we've reached this intersection, we know rindex_from: it's that of the last containing shape.
        if ( i == *this ) {
            if ( containing_shapes.empty() ) {
                ret.rindex_from = 1.0; // If we didn't pass through any shapes, assume we're in a vacuum.
            } else {
                ret.rindex_from = containing_shapes.back()->getMaterial().getRefractiveIndex();
            }
        }

        // If this intersection's object is already in containing_shapes, we must be exiting it.
        // Remove it from containing_shapes.
        auto found = find( containing_shapes.begin(), containing_shapes.end(), i.obj );
        if ( found != containing_shapes.end() ) {
            containing_shapes.erase(found);
        }
        // Otherwise, add it to containing_shapes.
        else {
            containing_shapes.push_back(i.obj);
        }

        // Again, if we've reached our own intersection, now we know rindex_to: it's the shape we just added.
        // Now that we have rindex_from and rindex_to, we can terminate the loop.
        if ( i == *this ) {
            if ( containing_shapes.empty() ) {
                ret.rindex_to = 1.0; // Again, assume a vacuum if we didn't pass through any shapes.
            } else {
                ret.rindex_to = containing_shapes.back()->getMaterial().getRefractiveIndex();
            }
            break; // We have our values: we're done
        }
    }

    return ret;
}
