#pragma once

#include "Ray.h"
#include "Shape.h"
#include <utility>
#include <memory>

class Cube final : public Shape {
public:
    static std::shared_ptr<Shape> make()
    {
        std::shared_ptr<Shape> ret(new Cube());
        return ret;
    }
    static std::shared_ptr<Shape> make(const Matrix &M)
    {
        std::shared_ptr<Shape> ret(new Cube(M));
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
    Cube() : Shape() { }
    Cube(const Matrix &M) : Shape(M) { }

    inline std::pair<double,double> check_axis(double origin, double direction);
    //inline void check_axis(bool sign, double origin, double invdir, double &tmin_out, double &tmax_out);
};
