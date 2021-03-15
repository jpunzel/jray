#pragma once

#include "Point.h"
#include "Ray.h"
#include <limits>
#include <utility>

class BoundingBox {
public:
    BoundingBox() : min(Point(std::numeric_limits<double>::infinity(),
                              std::numeric_limits<double>::infinity(),
                              std::numeric_limits<double>::infinity() )),
                    max(Point(-std::numeric_limits<double>::infinity(),
                              -std::numeric_limits<double>::infinity(),
                              -std::numeric_limits<double>::infinity() ))
                    { }
    BoundingBox(const Point &min, const Point &max) : min(min), max(max) { }

    static const Point infinityPoint;
    static const Point minusInfinityPoint;

    void add(const Point &pt);
    void add(const BoundingBox &bb);
    bool contains(const Point &pt) const;
    bool contains(const BoundingBox &bb) const;

    BoundingBox transform(const Matrix &M);
    std::pair<BoundingBox,BoundingBox> splitBounds() const;
    bool intersects(const Ray &ray) const;
    Point min, max;

private:
    inline std::pair<double,double> check_axis(double origin, double direction, double min, double max) const;
};
