#pragma once

#include "Ray.h"
#include "Shape.h"
#include <utility>
#include <memory>
#include <vector>

typedef std::vector<std::shared_ptr<Shape>> shapePtrVec;

class Group final : public Shape {
public:
    static std::shared_ptr<Group> make()
    {
        std::shared_ptr<Group> ret(new Group());
        return ret;
    }
    static std::shared_ptr<Group> make(const Matrix &M)
    {
        std::shared_ptr<Group> ret(new Group(M));
        return ret;
    }
    static std::shared_ptr<Group> make(const shapePtrVec &children)
    {
        std::shared_ptr<Group> ret(new Group(children));
        return ret;
    }
    bool localIntersect(const Ray &ray, Iset &iset_out) override;
    Vector localNormalAt(const Point &obj_p, const Intersection *const ip) const override; // should never be called on a group

    bool isEmpty() const { return children.empty(); }
    void addChild(const std::shared_ptr<Shape> &shape) {
        children.push_back(shape);
        auto selfptr = shared_from_this();
        //auto selfptr = std::dynamic_pointer_cast<Group>(shared_from_this());
        shape->setParent(selfptr);
        BoundingBox pbounds = shape->parentBounds();
        bbox.add(pbounds);

        // update bbox on each parent in the chain
        auto p = selfptr;
        auto c = selfptr;
        while ( (p = p->getParent()) != nullptr) {
            pbounds = pbounds.transform(c->getTransform()); // get bounding box in next parent space
            p->bbox.add(pbounds);
            c = c->getParent();
        }

    }
    shapePtrVec getChildren() const {
        return children;
    }

    // This is a very expensive function to call because it involves
    // doing space transforms on every child's bounds. Only used in constructor to
    // re-create bounding box from child list. Otherwise just use local bbox member.
    BoundingBox bounds() const override {
        BoundingBox box = BoundingBox();
        for (auto c : children) {
            BoundingBox cbox = c->parentBounds();
            box.add(cbox);
        }
        return box;
    }
    bool includes(const std::shared_ptr<Shape> &shape) {
        for ( auto c : children ) {
            if ( c->includes(shape) )
                return true;
        }
        return false;
    }

    // Recursive function divides group children into subgroups. Essentially creates
    // a BVH from this Group.
    void divide(size_t threshold) override;

    // partitionChildren() is used by divide() to create the BVH.
    // Returns a pair of lists of children corresponding to the split bbox.
    // Note: this will remove the child shapes that it partitions.
    std::pair<shapePtrVec,shapePtrVec> partitionChildren();

private:
    Group() : Shape() { }
    Group(const Matrix &M) : Shape(M) { }
    Group(const shapePtrVec &children) : Shape(), children(children) { bbox = bounds(); }

    shapePtrVec children;
};
