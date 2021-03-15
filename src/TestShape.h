#pragma once

#include "Ray.h"
#include "Shape.h"
#include "util.h"
#include <utility>
#include <memory>
#include <limits>

class TestShape : public Shape {
public:
    static std::shared_ptr<Shape> make()
    {
        std::shared_ptr<Shape> ret(new TestShape());
        return ret;
    }
    static std::shared_ptr<Shape> make(const Matrix &M)
    {
        std::shared_ptr<Shape> ret(new TestShape(M));
        return ret;
    }
    bool localIntersect(const Ray &ray, Iset &iset_out) override;
    Vector localNormalAt(const Point &obj_p, const Intersection *const ip) const override;
    BoundingBox bounds() const override {
        return BoundingBox( Point(-EPSILON,-EPSILON,-EPSILON),Point(EPSILON,EPSILON,EPSILON) );
    }

    bool includes(const std::shared_ptr<Shape> &shape) { return shape == shared_from_this(); }
    void divide(size_t threshold) override { }

    std::shared_ptr<Ray> saved_ray;
    
private:
    TestShape() : Shape() { }
    TestShape(const Matrix &M) : Shape(M) { }
};
