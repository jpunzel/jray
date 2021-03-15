#pragma once

#include "Ray.h"
#include "Shape.h"
#include <utility>
#include <memory>
#include <vector>

#define CSG_UNION 0
#define CSG_INTERSECT 1
#define CSG_DIFFERENCE 2

class CSG final : public Shape {
public:
    static std::shared_ptr<CSG> make(unsigned short op)
    {
        std::shared_ptr<CSG> ret(new CSG(op));
        return ret;
    }
    static std::shared_ptr<CSG> make(unsigned short op, std::shared_ptr<Shape> left, std::shared_ptr<Shape> right)
    {
        std::shared_ptr<CSG> ret(new CSG(op));
        ret->addChildren(left, right);
        return ret;
    }
    static std::shared_ptr<CSG> make(unsigned short op, std::shared_ptr<Shape> left, std::shared_ptr<Shape> right, const Matrix &M)
    {
        std::shared_ptr<CSG> ret(new CSG(op, M));
        ret->addChildren(left, right);
        return ret;
    }
    bool localIntersect(const Ray &ray, Iset &iset_out) override;
    Vector localNormalAt(const Point &obj_p, const Intersection *const ip) const override; // should never be called on a CSG

    BoundingBox bounds() const override {
        BoundingBox box = BoundingBox();
        for (auto c : { left, right } ) {
            BoundingBox cbox = c->parentBounds();
            box.add(cbox);
        }
        return box;
    }

    bool includes(const std::shared_ptr<Shape> &shape) {
        if ( left->includes(shape) )
            return true;
        else if ( right->includes(shape) )
            return true;
        return false;
    }

    void divide(size_t threshold) override {
        for (auto c : { left, right } ) {
            c->divide(threshold);
        }
    }

    void addChildren(const std::shared_ptr<Shape> &l, const std::shared_ptr<Shape> &r) {
        left = l;
        right = r;
        auto selfptr = shared_from_this();
        left->setParent(selfptr);
        right->setParent(selfptr);
        bbox.add(left->parentBounds());
        bbox.add(right->parentBounds());

        update_parent_chain_bbox();
    }

    // always add left before right
    void addChild(const std::shared_ptr<Shape> &s) {
        if (left == nullptr) {
            left = s;
        } else if (right == nullptr) {
            right = s;
        } else {
            throw std::logic_error("Attempted to add third child shape to CSG");
        }
        auto selfptr = shared_from_this();
        s->setParent(selfptr);
        bbox.add(s->parentBounds());
        
        update_parent_chain_bbox();
    }

    unsigned short op;
    std::shared_ptr<Shape> left;
    std::shared_ptr<Shape> right;

private:
    // save initialization of left, right for the public shared_ptr make() constructors,
    // because we first need a shared_ptr to this object in order to set their parent
    CSG(unsigned short op) : Shape(), op(op) { }
    CSG(unsigned short op, const Matrix &M) : Shape(M), op(op) { }
    bool filter_intersections(const Iset &in, Iset &out);
    bool intersection_allowed(bool lhit, bool inl, bool inr);

    // updates bbox on each parent in the chain
    void update_parent_chain_bbox() {
        auto selfptr = shared_from_this();
        auto p = selfptr;
        auto c = selfptr;
        BoundingBox pbounds;
        while ( (p = p->getParent()) != nullptr) {
            pbounds = pbounds.transform(c->getTransform()); // get bounding box in next parent space
            p->bbox.add(pbounds);
            c = c->getParent();
        }
    }
};
