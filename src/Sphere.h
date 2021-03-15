#pragma once

#include "Ray.h"
#include "Shape.h"
#include <utility>
#include <memory>

class Sphere final : public Shape {
public:
    static std::shared_ptr<Shape> make()
    {
        std::shared_ptr<Shape> ret(new Sphere());
        return ret;
    }
    static std::shared_ptr<Shape> make(const Matrix &M)
    {
        std::shared_ptr<Shape> ret(new Sphere(M));
        return ret;
    }
    bool localIntersect(const Ray &ray, Iset &iset_out) override;
    Vector localNormalAt(const Point &obj_p, const Intersection *const ip) const override;
    BoundingBox bounds() const override {
        return BoundingBox( Point(-1,-1,-1) , Point(1,1,1) );
    }

    void divide(size_t threshold) override { }
    bool includes(const std::shared_ptr<Shape> &shape) { return shape == shared_from_this(); }    

private:
    Sphere() : Shape() { }
    Sphere(const Matrix &M) : Shape(M) { }
};
