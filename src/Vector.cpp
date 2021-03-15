#include "Vector.h"

double dot(const Vector &v1, const Vector &v2)
{
    return v1.data[0]*v2.data[0] +
           v1.data[1]*v2.data[1] +
           v1.data[2]*v2.data[2] + 
           v1.data[3]*v2.data[3];
}

Vector cross(const Vector &v1, const Vector &v2)
{
    Vector ret(
                v1.data[1]*v2.data[2] - v1.data[2]*v2.data[1] ,
              -(v1.data[0]*v2.data[2] - v1.data[2]*v2.data[0]),
                v1.data[0]*v2.data[1] - v1.data[1]*v2.data[0]
              );
    return ret;
}

double Vector::length() const
{
    return sqrt( data[0]*data[0] + data[1]*data[1] + data[2]*data[2] );
}

double Vector::squared_length() const
{
    return data[0]*data[0] + data[1]*data[1] + data[2]*data[2];
}

Vector Vector::normalize() const
{
    return *this / length();
}
Vector Vector::reflect(const Vector &n) const
{
    return *this - n * 2 * dot(*this, n);
}

// Non-member functions

Vector normalize(const Vector &v)
{
    return v / v.length();
}
Vector reflect(const Vector &v, const Vector &n)
{
    return v - n * 2 * dot(v, n);
}

Vector operator/(double f, const Vector& v)
{
    return Vector( f / v.data[0], f / v.data[1], f / v.data[2] );
}