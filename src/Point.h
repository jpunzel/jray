#pragma once
#include "Tuple.h"

class Point : public Tuple
{
public:
    Point() {
        data[0] = data[1] = data[2] = 0;
        data[3] = 1;
    }
    Point(double x, double y, double z) {
        data[0] = x;
        data[1] = y;
        data[2] = z;
        data[3] = 1; // w=1 is what defines a point
    }
    // Derived-to-base conversion
    Point(const Tuple &t) {
        data[0] = t.x();
        data[1] = t.y();
        data[2] = t.z();
        data[3] = 1;
    }

    friend std::istream& operator>>(std::istream &is, Point &p) {
        is >> p.data[0] >> p.data[1] >> p.data[2];
        return is;
    }

};