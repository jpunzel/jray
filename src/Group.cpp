#include "Group.h"
#include "BoundingBox.h"
#include "util.h"
#include <math.h>
#include <memory>


bool Group::localIntersect(const Ray &ray, Iset &iset_out) 
{
    if ( ! bbox.intersects(ray) ) {
        return false;
    }
    bool hit = false;
    for (auto c : children) {
        if ( c->intersect(ray, iset_out) ) {
            hit = true;
        }
    }
    return hit;
}

Vector Group::localNormalAt(const Point &obj_p, const Intersection *const ip) const
{
    throw std::logic_error("Cannot call localNormalAt on group");
}

void Group::divide(size_t threshold)
{
    if (threshold <= children.size() ) {
        std::pair<shapePtrVec,shapePtrVec> parts = partitionChildren();
        shapePtrVec left = std::get<0>(parts);
        shapePtrVec right = std::get<1>(parts);
        if ( left.size() > 0 )
            addChild(Group::make(left));
        if ( right.size() > 0 )
            addChild(Group::make(right));
    }

    for ( auto c : children ) {
        c->divide(threshold);
    }
}

std::pair<shapePtrVec,shapePtrVec> Group::partitionChildren()
{
    auto split = bbox.splitBounds();
    BoundingBox left = std::get<0>(split);
    BoundingBox right = std::get<1>(split);

    shapePtrVec left_children, right_children;

    for ( auto it = children.begin(); it != children.end(); ) {
        if (left.contains((*it)->parentBounds())) {
            left_children.push_back(*it);
            children.erase(it);
        }
        else if (right.contains((*it)->parentBounds())) {
            right_children.push_back(*it);
            children.erase(it);
        } else {
            ++it;
        }
    } 

    return std::pair<shapePtrVec,shapePtrVec>(left_children,right_children);
}