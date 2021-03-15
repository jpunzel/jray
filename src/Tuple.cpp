#include "Tuple.h"
#include "Matrix.h"


Tuple::Tuple(double x, double y, double z, double w) 
{
    data[0] = x;
    data[1] = y;
    data[2] = z;
    data[3] = w;
}

const double& Tuple::operator[](int i) const
{
    if (i < 0 || i > 3) {
        throw std::out_of_range("Index out of range");
    }
    return data[i];
}

double& Tuple::operator[](int i)
{
    if (i < 0 || i > 3) {
        throw std::out_of_range("Index out of range");
    }
    return data[i];
}


Tuple Tuple::operator-() const
{
    Tuple ret(-data[0], -data[1], -data[2], -data[3]);
    if (ret.w() < 0) {
        throw std::logic_error("Negated a point");
    }
    return ret;
}

Tuple& Tuple::operator+=(const Tuple &t2)
{
    data[0] += t2.data[0];
    data[1] += t2.data[1];
    data[2] += t2.data[2];
    data[3] += t2.data[3];
    if ( !valid(*this) ) {
        throw new std::logic_error("Invalid tuple type");
    }
    return *this;
}
Tuple& Tuple::operator-=(const Tuple &t2)
{
    data[0] -= t2.data[0];
    data[1] -= t2.data[1];
    data[2] -= t2.data[2];
    data[3] -= t2.data[3];
    if ( !valid(*this) ) {
        throw new std::logic_error("Invalid tuple type");
    }
    return *this;
}
Tuple& Tuple::operator*=(const Tuple &t2)
{
    data[0] *= t2.data[0];
    data[1] *= t2.data[1];
    data[2] *= t2.data[2];
    data[3] *= t2.data[3];
    if ( !valid(*this) ) {
        throw new std::logic_error("Invalid tuple type");
    }
    return *this;
}
Tuple& Tuple::operator/=(const Tuple &t2)
{
    data[0] /= t2.data[0];
    data[1] /= t2.data[1];
    data[2] /= t2.data[2];
    data[3] /= t2.data[3];
    if ( !valid(*this) ) {
        throw new std::logic_error("Invalid tuple type");
    }
    return *this;
}
Tuple& Tuple::operator*=(const double f)
{
    data[0] *= f;
    data[1] *= f;
    data[2] *= f;
    data[3] *= f;
    if ( !valid(*this) ) {
        throw new std::logic_error("Invalid tuple type");
    }
    return *this;
}
Tuple& Tuple::operator/=(const double f)
{
    data[0] /= f;
    data[1] /= f;
    data[2] /= f;
    data[3] /= f;
    if ( !valid(*this) ) {
        throw new std::logic_error("Invalid tuple type");
    }
    return *this;
}

Tuple& Tuple::operator*=(const Matrix &m)
{
    if (m.cols() != 4) { // Tuple has fixed size of 4
        throw std::invalid_argument("Cannot multiply: Matrix has wrong dimensions");
    }
    data[0] = m.data[0][0]*data[0] + m.data[0][1]*data[1] + m.data[0][2]*data[2] + m.data[0][3]*data[3];
    data[1] = m.data[1][0]*data[0] + m.data[1][1]*data[1] + m.data[1][2]*data[2] + m.data[1][3]*data[3];
    data[2] = m.data[2][0]*data[0] + m.data[2][1]*data[1] + m.data[2][2]*data[2] + m.data[2][3]*data[3];
    data[3] = m.data[3][0]*data[0] + m.data[3][1]*data[1] + m.data[3][2]*data[2] + m.data[3][3]*data[3];

    return *this;
}

Tuple Tuple::translate(double x, double y, double z)
{
    Matrix T = Matrix::translation(x,y,z);
    return (T * (*this));
}
Tuple Tuple::scale(double x, double y, double z)
{
    Matrix S = Matrix::scaling(x,y,z);
    return (S * (*this));
}
Tuple Tuple::rotate_x(double radians)
{
    Matrix R = Matrix::rotation_x(radians);
    return (R * (*this));
}
Tuple Tuple::rotate_y(double radians)
{
    Matrix R = Matrix::rotation_y(radians);
    return (R * (*this));
}
Tuple Tuple::rotate_z(double radians)
{
    Matrix R = Matrix::rotation_z(radians);
    return (R * (*this));
}
Tuple Tuple::shear(double x_y, double x_z, double y_x, double y_z, double z_x, double z_y)
{
    Matrix S = Matrix::shearing(x_y, x_z, y_x, y_z, z_x, z_y);
    return (S * (*this));
}







/* Non-member functions */

bool operator==(const Tuple &t1, const Tuple &t2)
{
    return ( doubleEqual(t1.data[0], t2.data[0]) &&
             doubleEqual(t1.data[1], t2.data[1]) &&
             doubleEqual(t1.data[2], t2.data[2]) &&
             doubleEqual(t1.data[3], t2.data[3]) );
}
bool operator!=(const Tuple &t1, const Tuple &t2)
{
    return !(t1 == t2);
}

Tuple operator+(const Tuple& t1, const Tuple& t2)
{
    Tuple ret( t1.data[0] + t2.data[0],
               t1.data[1] + t2.data[1],
               t1.data[2] + t2.data[2],
               t1.data[3] + t2.data[3] );
    if ( ! Tuple::valid(ret) ) {
        throw new std::logic_error("Invalid Tuple type");
    }
    return ret;
}
Tuple operator-(const Tuple& t1, const Tuple& t2)
{
    Tuple ret( t1.data[0] - t2.data[0],
               t1.data[1] - t2.data[1],
               t1.data[2] - t2.data[2],
               t1.data[3] - t2.data[3] );
    if ( ! Tuple::valid(ret) ) {
        throw new std::logic_error("Invalid Tuple type");
    }
    return ret;
}
Tuple operator*(const Tuple& t1, const Tuple& t2)
{
    Tuple ret( t1.data[0] * t2.data[0],
               t1.data[1] * t2.data[1],
               t1.data[2] * t2.data[2],
               t1.data[3] * t2.data[3] );
    if ( ! Tuple::valid(ret) ) {
        throw new std::logic_error("Invalid Tuple type");
    }
    return ret;
}
Tuple operator/(const Tuple& t1, const Tuple& t2)
{
    Tuple ret( t1.data[0] / t2.data[0],
               t1.data[1] / t2.data[1],
               t1.data[2] / t2.data[2],
               t1.data[3] / t2.data[3] );
    if ( ! Tuple::valid(ret) ) {
        throw new std::logic_error("Invalid Tuple type");
    }
    return ret;
}

Tuple operator*(const Tuple& t, double f)
{
    Tuple ret( t.data[0] * f, t.data[1] * f, t.data[2] * f, t.data[3] * f );
    if ( ! Tuple::valid(ret) ) {
        throw new std::logic_error("Invalid tuple type");
    }
    return ret;
}
Tuple operator*(double f, const Tuple& t)
{
    Tuple ret( t.data[0] * f, t.data[1] * f, t.data[2] * f, t.data[3] * f );
    if ( ! Tuple::valid(ret) ) {
        throw new std::logic_error("Invalid tuple type");
    }
    return ret;
}
Tuple operator/(const Tuple& t, double f)
{
    if ( f == 0 ) {
        throw new std::runtime_error("Attempted to divide Tuple by zero");
    }
    Tuple ret( t.data[0] / f, t.data[1] / f, t.data[2] / f, t.data[3] / f );
    if ( ! Tuple::valid(ret) ) {
        throw new std::logic_error("Invalid tuple type");
    }
    return ret;
}

std::istream& operator>>(std::istream &is, Tuple &t) {
    is >> t.data[0] >> t.data[1] >> t.data[2] >> t.data[3];
    return is;
}
std::ostream& operator<<(std::ostream &os, const Tuple &t) {
    os << "[" << t.data[0] << "," << t.data[1] << "," << t.data[2] << "," << t.data[3] << "]";
    return os;
}