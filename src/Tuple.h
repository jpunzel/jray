#pragma once

#include <cassert>
#include <math.h>
#include <exception>
#include <iostream>
#include <math.h>
#include "util.h"

class Matrix;

class Tuple
{
public:

    Tuple() = default;
    Tuple(double x, double y, double z, double w);

    double x() const { return data[0]; };
    double y() const { return data[1]; };
    double z() const { return data[2]; };
    double w() const { return data[3]; };

    const double& operator[](int i) const;
    double& operator[](int i);

    const Tuple& operator+() const { return *this; }
    Tuple operator-() const;

    Tuple& operator+=(const Tuple &t2);
    Tuple& operator-=(const Tuple &t2);
    Tuple& operator*=(const Tuple &t2);
    Tuple& operator/=(const Tuple &t2);
    Tuple& operator*=(const double f);
    Tuple& operator/=(const double f);
    
    friend bool operator==(const Tuple &t1, const Tuple &t2);
    friend bool operator!=(const Tuple &t1, const Tuple &t2);

    // Define all binary operators for two Tuples
    friend Tuple operator+(const Tuple& t1, const Tuple& t2);
    friend Tuple operator-(const Tuple& t1, const Tuple& t2);
    friend Tuple operator*(const Tuple& t1, const Tuple& t2);
    friend Tuple operator/(const Tuple& t1, const Tuple& t2);
    // Scalar vector arithmetic
    friend Tuple operator*(const Tuple& t, double f);
    friend Tuple operator*(double f, const Tuple& t);
    friend Tuple operator/(const Tuple& t, double f);

    // Matrix-tuple multiply
    friend Tuple operator*(const Matrix &m, const Tuple &t);
    Tuple& operator*=(const Matrix &m);

    friend std::istream& operator>>(std::istream &is, Tuple &t);
    friend std::ostream& operator<<(std::ostream &os, const Tuple &t);

    Tuple translate(double x, double y, double z);
    Tuple scale(double x, double y, double z);
    Tuple rotate_x(double radians);
    Tuple rotate_y(double radians);
    Tuple rotate_z(double radians);
    Tuple shear(double x_y, double x_z, double y_x, double y_z, double z_x, double z_y);

    static bool valid(const Tuple &t)
    {
        return ( t.w() <= 1.0 && t.w() >= 0.0 );
    }

protected:
    double data[4];
    
};
