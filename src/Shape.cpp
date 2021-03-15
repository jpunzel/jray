#include "Shape.h"
#include "Group.h"

// This simple intersect function defers almost all work to the
// concrete localIntersect implementation of the derived classes.
// The only thing we do first is transform the ray to object space.
bool Shape::intersect(const Ray &r, Iset &iset_out)
{
    Ray ray_transformed = r.transform(inverse_transform);

    return localIntersect(ray_transformed, iset_out);
}

Vector Shape::normalAt(const Point &p, const Intersection *const ip) const
{
    //Point obj_p = inverse_transform * p;
    Point obj_p = world_to_object(p);
    Vector obj_normal = localNormalAt(obj_p, ip);
    //Vector world_normal = inverse_transform.transpose() * obj_normal;
    //world_normal[3] = 0; // Make it explicit that world_normal is a vector
    //return world_normal.normalize();
    return normal_to_world(obj_normal);
}

Point Shape::world_to_object(Point p) const
{
    if (parent) {
        p = parent->world_to_object(p); // recursively undo parent group transforms
    }
    return inverse_transform * p;
}

Vector Shape::normal_to_world(Vector n) const
{
    n = inverse_transform.transpose() * n;
    n[3] = 0; // ensure w=0 afterwards
    n = n.normalize();

    if (parent) {
        n = parent->normal_to_world(n);
    }

    return n;
}

Material Shape::getMaterial() const
{
    // first use nearest modified material in parent group hierarchy,
    // otherwise use this object's own material which may have been
    // default constructed
    auto p = parent;
    while ( p ) {
        if ( p->material_modified )
            return p->getMaterial();
        p = p->parent;
    }

    return material;
}