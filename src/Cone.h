#pragma once

#include "Ray.h"
#include "Shape.h"
#include <utility>
#include <memory>
#include <limits>
#include <math.h>

class Cone final : public Shape {
public:
    static std::shared_ptr<Shape> make()
    {
        std::shared_ptr<Shape> ret(new Cone());
        return ret;
    }
    static std::shared_ptr<Shape> make(const Matrix &M)
    {
        std::shared_ptr<Shape> ret(new Cone(M));
        return ret;
    }
    static std::shared_ptr<Shape> make(const Matrix &M, double min_y, double max_y)
    {
        std::shared_ptr<Shape> ret(new Cone(M, min_y, max_y));
        return ret;
    }
    static std::shared_ptr<Shape> make(const Matrix &M, double min_y, double max_y, bool closed)
    {
        std::shared_ptr<Shape> ret(new Cone(M, min_y, max_y, closed));
        return ret;
    }
    static std::shared_ptr<Shape> make(double min_y, double max_y)
    {
        std::shared_ptr<Shape> ret(new Cone(min_y, max_y));
        return ret;
    }
    static std::shared_ptr<Shape> make(double min_y, double max_y, bool closed)
    {
        std::shared_ptr<Shape> ret(new Cone(min_y, max_y, closed));
        return ret;
    }
    bool localIntersect(const Ray &ray, Iset &iset_out) override;
    Vector localNormalAt(const Point &obj_p, const Intersection *const ip) const override;
    BoundingBox bounds() const override {
        double limit = std::max( abs(min_y), abs(max_y) );
        return BoundingBox(Point(-limit, min_y, -limit), Point(limit, max_y, limit));
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
    Cone() : Shape(),
                 min_y(-std::numeric_limits<double>::infinity()),
                 max_y(std::numeric_limits<double>::infinity()),
                 is_closed(false) { }
    Cone(double min_y, double max_y) : Shape(), min_y(min_y), max_y(max_y), is_closed(false) { }
    Cone(double min_y, double max_y, bool closed) : Shape(), min_y(min_y), max_y(max_y), is_closed(closed) { }
    Cone(const Matrix &M) : Shape(M),
                                min_y(-std::numeric_limits<double>::infinity()),
                                max_y(std::numeric_limits<double>::infinity()) { }
    Cone(const Matrix &M, double min_y, double max_y) : Shape(M), min_y(min_y), max_y(max_y), is_closed(false) { }
    Cone(const Matrix &M, double min_y, double max_y, bool closed) : Shape(M), min_y(min_y), max_y(max_y), is_closed(closed) { }

    inline bool check_cap(const Ray &ray, double t, double y);
    inline bool intersect_caps(const Ray &ray, Iset &iset_out);

    double min_y;
    double max_y;
    bool is_closed;
};
