#pragma once
#include "util.h"
#include "Vector.h"
#include "Point.h"


class Matrix {

public:
    Matrix(unsigned r, unsigned c);
    Matrix(); // default constructor is a 4x4 Matrix, which is our most common use case
    ~Matrix();
    Matrix(unsigned r, unsigned c, std::initializer_list<double> list);
    Matrix(const Matrix&);
    Matrix& operator=(const Matrix&);

    // Access elements via call operator
    double& operator()(unsigned x, unsigned y);
    double operator()(unsigned x, unsigned y) const;

    Matrix& operator+=(const Matrix &B);
    Matrix& operator-=(const Matrix &B);
    Matrix& operator*=(const Matrix &B);
    Matrix& operator*=(double f);
    Matrix& operator/=(double f);

    friend Tuple operator*(const Matrix &m, const Tuple &t);

    friend std::ostream& operator<<(std::ostream& os, const Matrix &m);
    friend std::istream& operator>>(std::istream& is, const Matrix &m);

    unsigned rows() const { return m_rows; }
    unsigned cols() const { return m_cols; }

    static const Matrix identity(unsigned sz);
    static const Matrix translation(double x, double y, double z);
    static const Matrix scaling(double x, double y, double z);
    static const Matrix rotation_x(double radians);
    static const Matrix rotation_y(double radians);
    static const Matrix rotation_z(double radians);
    static const Matrix shearing(double x_y, double x_z, double y_x, double y_z, double z_x, double z_y);
    bool isIdentity() const;
    Matrix transpose() const;

    Matrix submatrix(unsigned x, unsigned y) const;
    double cofactor(unsigned r, unsigned c) const;
    double det() const;
    Matrix inverse() const;
    Matrix translate(double x, double y, double z) const;
    Matrix scale(double x, double y, double z) const;
    Matrix rotate_x(double radians) const;
    Matrix rotate_y(double radians) const;
    Matrix rotate_z(double radians) const;
    Matrix shear(double x_y, double x_z, double y_x, double y_z, double z_x, double z_y) const;

private:
    double ** data;
    unsigned m_rows;
    unsigned m_cols;
    friend class Tuple;

};

bool operator==(const Matrix &m1, const Matrix &m2);
bool operator!=(const Matrix &m1, const Matrix &m2);

Matrix operator+(const Matrix &m1, const Matrix &m2);
Matrix operator-(const Matrix &m1, const Matrix &m2);
Matrix operator*(const Matrix &m1, const Matrix &m2);
Matrix operator*(const Matrix &m1, double f);
Matrix operator*(double f, const Matrix &m1);
Matrix operator/(const Matrix& m1, double f);
