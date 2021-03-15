#include "Matrix.h"

Matrix::Matrix(unsigned r, unsigned c): m_rows(r), m_cols(c)
{
    data = create2DArray<double>(r, c);
}

Matrix::Matrix(): Matrix(4,4) { }

Matrix::Matrix(unsigned r, unsigned c, std::initializer_list<double> list): m_rows(r), m_cols(c)
{
    data = create2DArray<double>(r, c);
    if (list.size() != r*c) {
        throw std::invalid_argument("Bad initializer list given to Matrix constructor");
    }
    auto itr = list.begin();
    for (int i = 0; i < m_rows; i++) {
        for (int j = 0; j < m_cols; j++) {
            data[i][j] = *itr++;
        }
    }
}

Matrix::~Matrix()
{
    delete2DArray(data);
}

Matrix::Matrix(const Matrix& rhs): m_rows(rhs.m_rows), m_cols(rhs.m_cols)
{
    data = create2DArray<double>(rhs.m_rows, rhs.m_cols);
    for (int i = 0; i < rhs.m_cols; i++) {
        for (int j = 0; j < rhs.m_rows; j++) {
            data[i][j] = rhs.data[i][j];
        }
    }
}

Matrix& Matrix::operator=(const Matrix& B)
{
    if (this == &B) { return *this; }
    if (m_rows != B.m_rows || m_cols != B.m_cols ) {
        delete2DArray(data);
        create2DArray<double>(B.m_rows, B.m_cols);
        m_rows = B.m_rows;
        m_cols = B.m_cols;
    }
    for (int i = 0; i < m_rows; i++) {
        for (int j = 0; j < m_cols; j++) {
            data[i][j] = B.data[i][j];
        }
    }
    return *this;
}

double& Matrix::operator()(unsigned x, unsigned y)
{
    if (x >= m_rows || y >= m_cols) {
        throw std::out_of_range("Bad matrix coords");
    }
    return data[x][y];
}

double Matrix::operator()(unsigned x, unsigned y) const
{
    if (x >= m_rows || y >= m_cols) {
        throw std::out_of_range("Bad matrix coords");
    }
    return data[x][y];
}


Matrix& Matrix::operator+=(const Matrix &B)
{
    if (m_rows != B.m_rows || m_cols != B.m_cols ) {
        throw std::invalid_argument("Cannot add matrices of different size");
    }
    for (int i = 0; i < m_rows; i++) {
        for (int j = 0; j < m_cols; j++) {
            data[i][j] += B.data[i][j];
        }
    }
    return *this;
}
Matrix& Matrix::operator-=(const Matrix &B)
{
    if (m_rows != B.m_rows || m_cols != B.m_cols ) {
        throw std::invalid_argument("Cannot subtract matrices of different size");
    }
    for (int i = 0; i < m_rows; i++) {
        for (int j = 0; j < m_cols; j++) {
            data[i][j] -= B.data[i][j];
        }
    }
    return *this;
}

Matrix& Matrix::operator*=(const Matrix &B)
{
    if (m_cols != B.m_rows) {
        throw std::invalid_argument("Cannot multiply matrices");
    }
    Matrix M(m_rows, B.m_cols);
    for (int i = 0; i < M.rows(); i++) {
        for (int j = 0; j < B.m_cols; j++) {
            for (int k = 0; k < m_cols; k++) {
                M(i,j) += ( data[i][k] * B.data[k][j] );
            }
        }
    }
    return (*this = M);
}
Matrix& Matrix::operator*=(double f)
{
    for (int i = 0; i < m_rows; i++) {
        for (int j = 0; j < m_cols; j++) {
            data[i][j] *= f;
        }
    }
    return *this;
}
Matrix& Matrix::operator/=(double f)
{
    for (int i = 0; i < m_rows; i++) {
        for (int j = 0; j < m_cols; j++) {
            data[i][j] /= f;
        }
    }
    return *this;
}

bool Matrix::isIdentity() const
{
    return doubleEqual(data[0][0] , 1) &&
           doubleEqual(data[1][1] , 1) &&
           doubleEqual(data[2][2] , 1) &&
           doubleEqual(data[3][3] , 1);
}

Matrix Matrix::transpose() const
{
    Matrix ret(m_rows,m_cols);
    for (int i = 0; i < m_rows; i++) {
        for (int j = 0; j < m_cols; j++) {
            ret(j,i) = data[i][j];
        }
    }
    return ret;
}

Matrix Matrix::submatrix(unsigned x, unsigned y) const
{
    Matrix ret(m_rows-1, m_cols-1);

    for (int i = 0, a = 0; i < m_rows; i++) {
        if (i == x) { continue; }
        for (int j = 0, b = 0; j < m_cols; j++) {
            if (j == y) { continue; }
            ret(a,b) = data[i][j];
            ++b;
        }
        ++a;
    }
    return ret;
}

double Matrix::cofactor(unsigned r, unsigned c) const
{
    double ret = submatrix(r,c).det();
    if ( (r+c) % 2 == 1 ) {
        return -ret;
    } else return ret;
}

double Matrix::det() const
{
    double det = 0;
    if ( m_rows != m_cols ) {
        throw std::logic_error("Cannot get determinant for non-square matrix");
    }
    if ( m_rows == 2 ) {
        det = data[0][0] * data[1][1] - data[0][1] * data[1][0];
    }
    else {
        for (int i = 0; i < m_cols; i++ ) {
            det += data[0][i] * cofactor(0,i);
        }
    }
    return det;
}

Matrix Matrix::inverse() const
{
    if ( det() == 0 ) {
        throw std::logic_error("Cannot inverse non-invertible matrix");
    }
    Matrix M(m_rows,m_cols);

    for (int i = 0; i < m_rows; i++) {
        for (int j = 0; j < m_cols; j++) {
            double c = cofactor(i,j);
            M(j,i) = c / det(); // switch i,j to skip extra transpose step
        }
    }
    return M;
}

Matrix Matrix::translate(double x, double y, double z) const
{
    if ( m_rows != 4 && m_cols !=4 ) {
        throw std::logic_error("Cannot translate non-4x4 matrix");
    }
    Matrix tr = translation(x,y,z);
    return (tr * (*this));
}
Matrix Matrix::scale(double x, double y, double z) const
{
    if ( m_rows != 4 && m_cols !=4 ) {
        throw std::logic_error("Cannot scale non-4x4 matrix");
    }
    Matrix s = scaling(x,y,z);
    return (s * (*this));
}
Matrix Matrix::rotate_x(double radians) const
{
    if ( m_rows != 4 && m_cols !=4 ) {
        throw std::logic_error("Cannot rotate non-4x4 matrix");
    }
    Matrix r = rotation_x(radians);
    return (r * (*this));
}
Matrix Matrix::rotate_y(double radians) const
{
    if ( m_rows != 4 && m_cols !=4 ) {
        throw std::logic_error("Cannot rotate non-4x4 matrix");
    }
    Matrix r = rotation_y(radians);
    return (r * (*this));
}
Matrix Matrix::rotate_z(double radians) const
{
    if ( m_rows != 4 && m_cols !=4 ) {
        throw std::logic_error("Cannot rotate non-4x4 matrix");
    }
    Matrix r = rotation_z(radians);
    return (r * (*this));
}
Matrix Matrix::shear(double x_y, double x_z, double y_x, double y_z, double z_x, double z_y) const
{
    if ( m_rows != 4 && m_cols !=4 ) {
        throw std::logic_error("Cannot shear non-4x4 matrix");
    }
    Matrix s = shearing(x_y, x_z, y_x, y_z, z_x, z_y);
    return (s * (*this));
}

/* Static functions */
const Matrix Matrix::identity(unsigned sz) 
{
    Matrix ret(sz, sz);
    for (int i = 0; i < sz; i++) {
        ret(i,i) = 1;
    }
    return ret;
}

const Matrix Matrix::translation(double x, double y, double z)
{
    Matrix tr = identity(4);
    tr(0,3) = x;
    tr(1,3) = y;
    tr(2,3) = z;
    return tr;
}
const Matrix Matrix::scaling(double x, double y, double z)
{
    Matrix s = identity(4);
    s(0,0) = x;
    s(1,1) = y;
    s(2,2) = z;
    return s;
}
const Matrix Matrix::rotation_x(double radians)
{
    Matrix r = identity(4);
    r(1,1) = cos(radians);
    r(1,2) = -sin(radians);
    r(2,1) = sin(radians);
    r(2,2) = cos(radians);
    return r;
}
const Matrix Matrix::rotation_y(double radians)
{
    Matrix r = identity(4);
    r(0,0) = cos(radians);
    r(0,2) = sin(radians);
    r(2,0) = -sin(radians);
    r(2,2) = cos(radians);
    return r;
}
const Matrix Matrix::rotation_z(double radians)
{
    Matrix r = identity(4);
    r(0,0) = cos(radians);
    r(0,1) = -sin(radians);
    r(1,0) = sin(radians);
    r(1,1) = cos(radians);
    return r;
}
const Matrix Matrix::shearing(double x_y, double x_z, double y_x, double y_z, double z_x, double z_y)
{
    Matrix s = identity(4);
    s(0,1) = x_y;
    s(0,2) = x_z;
    s(1,0) = y_x;
    s(1,2) = y_z;
    s(2,0) = z_x;
    s(2,1) = z_y;
    return s;
}

/* Non-member functions */

bool operator==(const Matrix& m1, const Matrix &m2)
{
    if (m1.rows() != m2.rows() || m1.cols() != m2.cols() ) {
        return false;
    }
    for (int i = 0; i < m1.rows(); i++) {
        for (int j = 0; j < m1.cols(); j++) {
            if (!doubleEqual(m1(i,j) , m2(i,j))) {
                return false;
            }
        }
    }
    return true;
}

bool operator!=(const Matrix& m1, const Matrix& m2)
{
    return !(m1 == m2);
}

std::ostream& operator<<(std::ostream &os, const Matrix &m)
{
    for (int i = 0; i < m.rows(); i++) {
        os << m.data[i][0];
        for (int j = 1; j < m.cols(); j++) {
            os << " ";
            os << m.data[i][j];
        }
        os << std::endl;
    }
    return os;
}

std::istream& operator>>(std::istream &is, const Matrix &m)
{
    for (int i = 0; i < m.rows(); i++ ) {
        for (int j = 0; j < m.cols(); j++ ) {
            is >> m.data[i][j];
        }
    }
    return is;
}


Tuple operator*(const Matrix &m, const Tuple &t)
{
    if (m.cols() != 4) { // Tuple has fixed size of 4
        throw std::invalid_argument("Cannot multiply: Matrix has wrong dimensions");
    }

//    return Tuple(
//        m(0,0)*t.x() + m(0,1)*t.y() + m(0,2)*t.z() + m(0,3)*t.w(),
//        m(1,0)*t.x() + m(1,1)*t.y() + m(1,2)*t.z() + m(1,3)*t.w(),
//        m(2,0)*t.x() + m(2,1)*t.y() + m(2,2)*t.z() + m(2,3)*t.w(),
//        m(3,0)*t.x() + m(3,1)*t.y() + m(3,2)*t.z() + m(3,3)*t.w()
//    );
    return Tuple(
        m.data[0][0]*t.data[0] + m.data[0][1]*t.data[1] + m.data[0][2]*t.data[2] + m.data[0][3]*t.data[3],
        m.data[1][0]*t.data[0] + m.data[1][1]*t.data[1] + m.data[1][2]*t.data[2] + m.data[1][3]*t.data[3],
        m.data[2][0]*t.data[0] + m.data[2][1]*t.data[1] + m.data[2][2]*t.data[2] + m.data[2][3]*t.data[3],
        m.data[3][0]*t.data[0] + m.data[3][1]*t.data[1] + m.data[3][2]*t.data[2] + m.data[3][3]*t.data[3]
    );
}

Matrix operator+(const Matrix &m1, const Matrix &m2) { return Matrix(m1) += m2; }
Matrix operator-(const Matrix &m1, const Matrix &m2) { return Matrix(m1) -= m2; }
Matrix operator*(const Matrix &m1, const Matrix &m2) { return Matrix(m1) *= m2; }
Matrix operator*(const Matrix &m1, double f) { return Matrix(m1) *= f; }
Matrix operator*(double f, const Matrix &m) { return m*f; }
Matrix operator/(const Matrix &m1, double f) { return Matrix(m1) /= f; }