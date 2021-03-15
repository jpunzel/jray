#pragma once

#include "Ray.h"
#include "Shape.h"
#include "util.h"
#include <utility>
#include <memory>

class Triangle final : public Shape {
public:
    static std::shared_ptr<Shape> make(const Point &p1, const Point &p2, const Point &p3)
    {
        std::shared_ptr<Shape> ret(new Triangle(p1,p2,p3));
        return ret;
    }
    static std::shared_ptr<Shape> make(const Point &p1, const Point &p2, const Point &p3, const Matrix &M)
    {
        std::shared_ptr<Shape> ret(new Triangle(p1,p2,p3,M));
        return ret;
    }
    static std::shared_ptr<Shape> make(const Point &p1, const Point &p2, const Point &p3, const Vector &n1, const Vector &n2, const Vector &n3)
    {
        std::shared_ptr<Shape> ret(new Triangle(p1,p2,p3,n1,n2,n3));
        return ret;
    }
    static std::shared_ptr<Shape> make(const Point &p1, const Point &p2, const Point &p3, const Vector &n1, const Vector &n2, const Vector &n3, const Matrix &M)
    {
        std::shared_ptr<Shape> ret(new Triangle(p1,p2,p3,n1,n2,n3,M));
        return ret;
    }
    bool localIntersect(const Ray &ray, Iset &iset_out) override;
    Vector localNormalAt(const Point &obj_p, const Intersection *const ip = nullptr) const override;
    BoundingBox bounds() const override {
        BoundingBox bb;
        bb.add(p1);
        bb.add(p2);
        bb.add(p3);
        return bb;
    }

    void divide(size_t threshold) override { }
    bool includes(const std::shared_ptr<Shape> &shape) { return shape == shared_from_this(); }

    Point p1, p2, p3;
    Vector e1, e2;
    bool isSmooth;
    Vector n1, n2, n3; // n2 and n3 only used on smooth triangles
    
private:
    Triangle(const Point &p1, const Point &p2, const Point &p3) : Shape() ,
                                                                  p1(p1), p2(p2), p3(p3),
                                                                  e1(p2-p1), e2(p3-p1),
                                                                  isSmooth(false),
                                                                  n1(normalize(cross(e2,e1))) { }
    Triangle(const Point &p1, const Point &p2, const Point &p3, const Matrix &M) : Shape(M),
                                                                                   p1(p1), p2(p2), p3(p3),
                                                                                   e1(p2-p1), e2(p3-p1),
                                                                                   isSmooth(false),
                                                                                   n1(normalize(cross(e2,e1))) { }
    Triangle(const Point &p1, const Point &p2, const Point &p3, const Vector &n1, const Vector &n2, const Vector &n3) : Shape() ,
                                                                  p1(p1), p2(p2), p3(p3),
                                                                  e1(p2-p1), e2(p3-p1),
                                                                  isSmooth(true),
                                                                  n1(n1), n2(n2), n3(n3) { }
    Triangle(const Point &p1, const Point &p2, const Point &p3, const Vector &n1, const Vector &n2, const Vector &n3, const Matrix &M) : Shape(M),
                                                                                   p1(p1), p2(p2), p3(p3),
                                                                                   e1(p2-p1), e2(p3-p1),
                                                                                   isSmooth(true),
                                                                                   n1(n1), n2(n2), n3(n3) { }
};
