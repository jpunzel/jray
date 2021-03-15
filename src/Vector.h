#pragma once

#include "Tuple.h"

class Vector : public Tuple
{
public:
    Vector() = default; 
    Vector(double x, double y, double z) {
        data[0] = x;
        data[1] = y;
        data[2] = z;
        data[3] = 0; // w=0 is what defines a vector
    }
    // Base-to-derived constructor
    Vector(const Tuple &t) : Tuple(t) {
        data[0] = t.x();
        data[1] = t.y();
        data[2] = t.z();
        data[3] = 0;
    }

    friend double dot(const Vector &v1, const Vector &v2);
    friend Vector cross(const Vector &v1, const Vector &v2);
    friend Vector normalize(const Vector &v);
    friend Vector reflect(const Vector &v, const Vector &n);

    // Add this divide operator which Tuple does not have
    friend Vector operator/(double f, const Vector& v);

    double length() const;
    double squared_length() const;
    Vector normalize() const;
    Vector reflect(const Vector &n) const;

    friend std::istream& operator>>(std::istream &is, Vector &v) {
        is >> v.data[0] >> v.data[1] >> v.data[2];
        return is;
    }

};

Vector normalize(const Vector &v);