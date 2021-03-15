#pragma once

#include "Ray.h"
#include "Shape.h"
#include <utility>
#include <memory>
#include <limits>

class Cylinder final : public Shape {
public:
    static std::shared_ptr<Shape> make()
    {
        std::shared_ptr<Shape> ret(new Cylinder());
        return ret;
    }
    static std::shared_ptr<Shape> make(const Matrix &M)
    {
        std::shared_ptr<Shape> ret(new Cylinder(M));
        return ret;
    }
    static std::shared_ptr<Shape> make(const Matrix &M, double min_y, double max_y)
    {
        std::shared_ptr<Shape> ret(new Cylinder(M, min_y, max_y));
        return ret;
    }
    static std::shared_ptr<Shape> make(const Matrix &M, double min_y, double max_y, bool closed)
    {
        std::shared_ptr<Shape> ret(new Cylinder(M, min_y, max_y, closed));
        return ret;
    }
    static std::shared_ptr<Shape> make(double min_y, double max_y)
    {
        std::shared_ptr<Shape> ret(new Cylinder(min_y, max_y));
        return ret;
    }
    static std::shared_ptr<Shape> make(double min_y, double max_y, bool closed)
    {
        std::shared_ptr<Shape> ret(new Cylinder(min_y, max_y, closed));
        return ret;
    }
    bool localIntersect(const Ray &ray, Iset &iset_out) override;
    Vector localNormalAt(const Point &obj_p, const Intersection *const ip) const override;
    BoundingBox bounds() const override {
        return BoundingBox( Point(-1, min_y, -1) , Point(1, max_y, 1) );
    }

    void divide(size_t threshold) override { }
    bool includes(const std::shared_ptr<Shape> &shape) { return shape == shared_from_this(); }

    double minimum() const { return min_y; }
    void minimum(double m) { min_y = m; }
    double maximum() const { return max_y; }
    void maximum(double m) { max_y = m; }
    bool closed() const { return is_closed; }
    void closed(bool c) { is_closed = c; }
    
private:
    Cylinder() : Shape(),
                 min_y(-std::numeric_limits<double>::infinity()),
                 max_y(std::numeric_limits<double>::infinity()),
                 is_closed(false) { }
    Cylinder(double min_y, double max_y) : Shape(), min_y(min_y), max_y(max_y), is_closed(false) { }
    Cylinder(double min_y, double max_y, bool closed) : Shape(), min_y(min_y), max_y(max_y), is_closed(closed) { }
    Cylinder(const Matrix &M) : Shape(M),
                                min_y(-std::numeric_limits<double>::infinity()),
                                max_y(std::numeric_limits<double>::infinity()) { }
    Cylinder(const Matrix &M, double min_y, double max_y) : Shape(M), min_y(min_y), max_y(max_y), is_closed(false) { }
    Cylinder(const Matrix &M, double min_y, double max_y, bool closed) : Shape(M), min_y(min_y), max_y(max_y), is_closed(closed) { }

    inline bool check_cap(const Ray &ray, double t);
    inline bool intersect_caps(const Ray &ray, Iset &iset_out);

    double min_y;
    double max_y;
    bool is_closed;
};
