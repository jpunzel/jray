#include "CSG.h"

bool CSG::localIntersect(const Ray &ray, Iset &iset_out) 
{
    Iset leftxs;
    Iset rightxs;
    left->intersect(ray, leftxs);
    right->intersect(ray, rightxs);

    Iset xs;
    std::merge(leftxs.begin(), leftxs.end(),
               rightxs.begin(), rightxs.end(),
               std::inserter(xs, xs.begin()));
    return filter_intersections(xs, iset_out);
}

Vector CSG::localNormalAt(const Point &obj_p, const Intersection *const ip) const
{
    throw std::logic_error("Cannot call localNormalAt on CSG");
}

bool CSG::filter_intersections(const Iset &in, Iset &out)
{
    bool inl = false;
    bool inr = false;
    bool ret = false;

    for (auto i: in) {
        bool lhit = left->includes(i.obj);
        if ( intersection_allowed(lhit, inl, inr) ) {
            out.insert(i);
            ret = true;
        }
        if (lhit) {
            inl = !inl;
        } else {
            inr = !inr;
        }
    }
    return ret;
}

// Implements a truth table for all three CSG operations
// lhit: true if left shape hit, false if right shape hit
// inl: true if hit occurs in the left shape
// inr: true if hit occurs in the right shape
bool CSG::intersection_allowed(bool lhit, bool inl, bool inr)
{
    if (op == CSG_UNION) {
        return (lhit && !inr) || (!lhit && !inl);
    }
    else if (op == CSG_INTERSECT) {
        return (lhit && inr) || (!lhit && inl);
    }
    else if (op == CSG_DIFFERENCE) {
        return (lhit && !inr) || (!lhit && inl);
    }
    return false;
}