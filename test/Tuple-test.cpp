#include "gtest/gtest.h"
#include "Tuple.h"
#include "Vector.h"
#include "Point.h"
#include <iostream>

TEST(TupleTest, addTwo) {
	Tuple a1 = Tuple(3, -2, 5, 1);
	Tuple a2 = Tuple(-2, 3, 1, 0);
	Tuple a3 = Tuple(1, 1, 6, 1);

	EXPECT_EQ(a1 + a2, a3);
}

TEST(TupleTest, pointIsTuple) {
	Tuple a = Tuple(4.3, -4.2, 3.1, 1.0);
	Point p = Point(4.3, -4.2, 3.1);
	EXPECT_EQ(a, p);
}

TEST(TupleTest, addPointToPoint) {
	Point a = Point(1, 1, 1);
	Point b = Point(1, 2, 1);
	EXPECT_ANY_THROW(a + b);
}

TEST(TupleTest, subtractVecFromPoint) {
	Vector v = Vector(1, 1, 1);
	Point p = Point(1, 1, 1);

	EXPECT_EQ(p - v, Tuple(0,0,0,1) );
}

TEST(TupleTest, subtractPointFromVec) {
	Vector v = Vector(1, 1, 1);
	Point p = Point(1, 1, 1);

	EXPECT_ANY_THROW(v - p);
}

TEST(TupleTest, negateVector) {
	Vector v = Vector(1, 1, 1);

	EXPECT_EQ(-v, Vector(-1, -1, -1) );
}

TEST(TupleTest, negatePoint) {
	Point p = Point(1, 1, 1);

	EXPECT_ANY_THROW(-p);
}

TEST(TupleTest, multiplyPoint) {
	Point p = Point(3, 2, 1);

	EXPECT_ANY_THROW(p * 2);
}

TEST(TupleTest, multiplyVector) {
	Vector v = Vector(1, 1, 1);
	EXPECT_EQ(v * 2, Vector(2,2,2));
}

TEST(TupleTest, multiplyVectorFraction) {
	Vector v = Vector(1,1,1);
	EXPECT_EQ(v * 0.5, Vector(0.5, 0.5, 0.5));
}

TEST(TupleTest, normalizeA)
{
	Vector v = Vector(4, 0, 0);
	EXPECT_EQ( v.normalize() , Vector(1,0,0));
}

TEST(TupleTest, normalizeB)
{
	Vector v = Vector(1, 2, 3);
	EXPECT_EQ( normalize(v), Vector(0.26726, 0.53452, 0.80178));
}

TEST(TupleTest, lengthOfNormalized)
{
	Vector v(1,2,3);
	Vector n = v.normalize();
	EXPECT_FLOAT_EQ( n.length(), 1.0 );
}

TEST(TupleTest, dotProduct)
{
	Vector a(1,2,3);
	Vector b(2,3,4);
	EXPECT_FLOAT_EQ( dot(a, b), 20 );
}

TEST(TupleTest, crossProduct)
{
	Vector a(1,2,3);
	Vector b(2,3,4);
	EXPECT_EQ( cross(a,b) , Vector(-1,2,-1) );
	EXPECT_EQ( cross(b,a) , Vector(1,-2,1) );
}

TEST(TupleTest, pointPlusVector)
{
	Point p(0,1,0);
	Vector v( 5.463348, 9.83427, 0);
	Point a = p + v;
	EXPECT_EQ( a, Point(5.463348 , 10.83427, 0) );
}