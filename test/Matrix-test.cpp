#include "gtest/gtest.h"
#include "Tuple.h"
#include "Vector.h"
#include "Point.h"
#include "Matrix.h"
#include <math.h>
#include <iostream>


TEST(MatrixTest, accessElements) {
    Matrix M = Matrix(4,4);

    M(2,2) = 99.9;
    EXPECT_FLOAT_EQ(M(2,2), 99.9 );
    EXPECT_ANY_THROW(M(100,100));
    EXPECT_ANY_THROW(M(4,4));
}

TEST(MatrixTest, compareTwo) {
    Matrix M1 = Matrix(4,4);
    Matrix M2 = Matrix(4,4);

    M1(0,0) = 2.0;
    M2(0,0) = 2.0;
    M1(2,3) = 1.0;
    M2(2,3) = 1.0;
    EXPECT_EQ(M1, M2);
    M2(3,3) = 4.0;
    EXPECT_NE(M1, M2);
}

TEST(MatrixTest, matrixMultiply) {
    Matrix A(4,4, {1,2,3,4,5,6,7,8,9,8,7,6,5,4,3,2});
    Matrix B(4,4, {-2,1,2,3,3,2,1,-1,4,3,6,5,1,2,7,8});
    Matrix C(4,4, {20,22,50,48,44,54,114,108,40,58,110,102,16,26,46,42});

    EXPECT_EQ(A*B, C);

}

TEST(MatrixTest, tupleMultiply) {
    Matrix A(4,4, {1,2,3,4,2,4,4,2,8,6,4,1,0,0,0,1} );
    Point b(1,2,3);

    EXPECT_EQ(A*b, Tuple(18,24,33,1));
}

TEST(MatrixTest, identity) {
    Matrix A = Matrix::identity(4);

    EXPECT_EQ(A, Matrix(4,4, {1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1}));
    EXPECT_TRUE(A.isIdentity());

    Matrix B(4,4, {0,1,2,4,1,2,4,8,2,4,8,16,4,8,16,32} );
    EXPECT_EQ( A*B, B);
}

TEST(MatrixTest, transpose) {
    Matrix A = Matrix(4,4, {0,9,3,0,9,8,0,8,1,8,5,3,0,0,5,8} );
    Matrix B = Matrix(4,4, {0,9,1,0,9,8,8,0,3,0,5,5,0,8,3,8} );
    EXPECT_EQ(A.transpose(), B);
}

TEST(MatrixTest, arthmetic) {
    Matrix A = Matrix(3,3, {1,2,3,1,2,3,1,2,3});
    Matrix B = A;
    Matrix C = Matrix(3,3, {2,4,6,2,4,6,2,4,6});
    Matrix D = Matrix(3,3, {6,12,18,6,12,18,6,12,18});

    EXPECT_EQ(A+B, C);
    EXPECT_EQ(C-B, A);
    EXPECT_EQ(A*B, D);
    EXPECT_EQ(C/2, B);
}

TEST(MatrixTest, submatrix) {
    Matrix A = Matrix(4,4, {-6,1,1,6,-8,5,8,6,-1,0,8,2,-7,1,-1,1} );
    EXPECT_EQ(A.submatrix(2,1) , Matrix(3,3, {-6,1,6,-8,8,6,-7,-1,1}));
}

TEST(MatrixTest, determinant) {
    Matrix A = Matrix(4,4, {-2,-8,3,5,-3,1,7,3,1,2,-9,6,-6,7,7,-9} );
    EXPECT_FLOAT_EQ(A.cofactor(0,0) , 690);
    EXPECT_FLOAT_EQ(A.cofactor(0,1) , 447);
    EXPECT_FLOAT_EQ(A.cofactor(0,2) , 210);
    EXPECT_FLOAT_EQ(A.cofactor(0,3) , 51);
    EXPECT_FLOAT_EQ(A.det(), -4071);
}

TEST(MatrixTest, inverse) {
    Matrix A = Matrix(4,4, {-5,2,6,-8,1,-5,1,8,7,7,-6,-7,1,-3,7,4});
    Matrix B = A.inverse();

    EXPECT_FLOAT_EQ(A.det(), 532);
    EXPECT_FLOAT_EQ(A.cofactor(2,3), -160.0);
    EXPECT_FLOAT_EQ(B(3,2) , -160/532.0);
    EXPECT_FLOAT_EQ(A.cofactor(3,2), 105.0);
    EXPECT_FLOAT_EQ(B(2,3), 105/532.0);
    EXPECT_EQ(B, Matrix(4,4, {0.21805,  0.45113,  0.24060,  -0.04511,
                             -0.80827, -1.45677, -0.44361,   0.52068,
                             -0.07895, -0.22368, -0.05263,   0.19737,
                             -0.52256, -0.81391, -0.30075,   0.30639} ) );

}

TEST(MatrixTest, inverseUndo) {
    Matrix A = Matrix(4,4, {3,-9,7,3,3,-8,2,-9,-4,4,4,1,-6,5,-1,1});
    Matrix B = Matrix(4,4, {8,2,2,2,3,-1,7,0,7,0,5,4,6,-2,0,5});
    Matrix C = A * B;

    EXPECT_EQ(C * B.inverse(), A);
}

TEST(MatrixTest, translate) {
    Matrix T = Matrix::translation(5,-3,2);
    Point p = Point(-3,4,5);

    EXPECT_EQ(T * p, Point(2,1,7) );
    EXPECT_EQ(T.inverse() * p, Point(-8,7,3) );
    // Translation does not affect vectors
    Vector v(-3,4,5);
    EXPECT_EQ(T * v, v);
}

TEST(MatrixTest, scale) {
    Matrix S = Matrix::scaling(2,3,4);
    Point p = Point(-4,6,8);
    Vector v = Vector(-4,6,8);
    EXPECT_EQ(S * p, Point(-8,18,32) );
    EXPECT_EQ(S * v, Vector(-8,18,32) );
    EXPECT_EQ(S.inverse() * v, Vector(-2,2,2) );
    Matrix R = Matrix::scaling(-1,1,1);
    Point p2 = Point(2,3,4);
    EXPECT_EQ(R * p2, Point(-2,3,4) );

}

TEST(MatrixTest, rotation_x) {
    Point p = Point(0,1,0);
    Matrix half_quarter = Matrix::rotation_x( PI/4 );
    Matrix full_quarter = Matrix::rotation_x( PI/2 );
    EXPECT_EQ(half_quarter * p, Point(0, sqrt(2)/2, sqrt(2)/2 ));
    EXPECT_EQ(full_quarter * p, Point(0,0,1) );
    EXPECT_EQ(half_quarter.inverse() * p, Point(0, sqrt(2)/2, -sqrt(2)/2));
}
TEST(MatrixTest, rotation_y) {
    Point p = Point(0,0,1);
    Matrix half_quarter = Matrix::rotation_y( PI/4 );
    Matrix full_quarter = Matrix::rotation_y( PI/2 );
    EXPECT_EQ(half_quarter * p, Point(sqrt(2)/2, 0, sqrt(2)/2 ));
    EXPECT_EQ(full_quarter * p, Point(1,0,0) );
}
TEST(MatrixTest, rotation_z) {
    Point p = Point(0,1,0);
    Matrix half_quarter = Matrix::rotation_z( PI/4 );
    Matrix full_quarter = Matrix::rotation_z( PI/2 );
    EXPECT_EQ(half_quarter * p, Point(-sqrt(2)/2, sqrt(2)/2, 0 ));
    EXPECT_EQ(full_quarter * p, Point(-1,0,0) );
}
TEST(MatrixTest, shearing) {
    Point p = Point(2,3,4);
    EXPECT_EQ( Matrix::shearing(1,0,0,0,0,0) * p , Point(5,3,4) );
    EXPECT_EQ( Matrix::shearing(0,1,0,0,0,0) * p , Point(6,3,4) );
    EXPECT_EQ( Matrix::shearing(0,0,1,0,0,0) * p , Point(2,5,4) );
    EXPECT_EQ( Matrix::shearing(0,0,0,1,0,0) * p , Point(2,7,4) );
    EXPECT_EQ( Matrix::shearing(0,0,0,0,1,0) * p , Point(2,3,6) );
    EXPECT_EQ( Matrix::shearing(0,0,0,0,0,1) * p , Point(2,3,7) );
}

TEST(MatrixTest, chainedTransform) {
    Point p = Point(1,0,1);
    Matrix A = Matrix::rotation_x( PI / 2 );
    Matrix B = Matrix::scaling(5,5,5);
    Matrix C = Matrix::translation(10, 5, 7);
    Point p2 = A * p;
    Point p3 = B * p2;
    Point p4 = C * p3;
    EXPECT_EQ(p4, Point(15,0,7) );
    Matrix T = C * B * A;
    EXPECT_EQ(T * p, Point(15,0,7) );
    Matrix I = Matrix::identity(4);
    EXPECT_EQ( I.rotate_x(PI / 2).scale(5,5,5).translate(10,5,7) * p, Point(15,0,7) );
}