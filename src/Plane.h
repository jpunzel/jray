#pragma once

#include "Ray.h"
#include "Shape.h"
#include <utility>
#include <memory>
#include <limits>

class Plane final : public Shape {
public:
    static std::shared_ptr<Shape> make()
    {
        std::shared_ptr<Shape> ret(new Plane());
        return ret;
    }
    static std::shared_ptr<Shape> make(const Matrix &M)
    {
        std::shared_ptr<Shape> ret(new Plane(M));
        return ret;
    }
    bool localIntersect(const Ray &ray, Iset &iset_out) override;
    Vector localNormalAt(const Point &obj_p, const Intersection *const ip) const override;
    BoundingBox bounds() const override {
        return BoundingBox( Point(-std::numeric_limits<double>::infinity(), 0, -std::numeric_limits<double>::infinity()),
                            Point(std::numeric_limits<double>::infinity(), 0, -std::numeric_limits<double>::infinity()));
    }

    void divide(size_t threshold) override { }
    bool includes(const std::shared_ptr<Shape> &shape) { return shape == shared_from_this(); } 

private:
    Plane() : Shape() { }
    Plane(const Matrix &M) : Shape(M) { }
};
