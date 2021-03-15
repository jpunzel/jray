#include "gtest/gtest.h"
#include "Tuple.h"
#include "Vector.h"
#include "Point.h"
#include "Ray.h"
#include "Plane.h"
#include <iostream>
#include <memory>

TEST(PlaneTest, planeHasConstantNormal) {
    auto p = Plane::make();
    Vector n1 = p->localNormalAt(Point(0,0,0));
    Vector n2 = p->localNormalAt(Point(10,0,-10));
    Vector n3 = p->localNormalAt(Point(-5,0,150));
    EXPECT_EQ(n1, Vector(0,1,0));
    EXPECT_EQ(n2, Vector(0,1,0));
    EXPECT_EQ(n3, Vector(0,1,0));
}

TEST(PlaneTest, intersectPlaneParallel) {
    auto p = Plane::make();
    Ray r = Ray(Point(0,10,0), Vector(0,0,1));
    Iset iset;
    bool hit = p->localIntersect(r, iset);
    EXPECT_FALSE(hit);
    EXPECT_TRUE(iset.empty());
}

TEST(PlaneTest, intersectCoplanar) {
    auto p = Plane::make();
    Ray r = Ray(Point(0,0,0), Vector(0,0,1));
    Iset iset;
    bool hit = p->localIntersect(r, iset);
    EXPECT_FALSE(hit);
    EXPECT_TRUE(iset.empty());
}

TEST(PlaneTest, intersectAbove) {
    auto p = Plane::make();
    Ray r = Ray(Point(0,1,0), Vector(0,-1,0));
    Iset iset;
    bool hit = p->localIntersect(r, iset);
    EXPECT_TRUE(hit);
    EXPECT_EQ(iset.size(), 1);
    EXPECT_FLOAT_EQ(iset.begin()->t, 1);
    EXPECT_EQ(iset.begin()->obj, p);
}

TEST(PlaneTest, intersectBelow) {
    auto p = Plane::make();
    Ray r = Ray(Point(0,-1,0), Vector(0,1,0));
    Iset iset;
    bool hit = p->localIntersect(r, iset);
    EXPECT_TRUE(hit);
    EXPECT_EQ(iset.size(), 1);
    EXPECT_FLOAT_EQ(iset.begin()->t, 1);
    EXPECT_EQ(iset.begin()->obj, p);
}