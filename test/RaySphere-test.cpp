#include "gtest/gtest.h"
#include "Tuple.h"
#include "Vector.h"
#include "Point.h"
#include "Ray.h"
#include "Sphere.h"
#include <iostream>
#include <memory>

TEST(RaySphereTest, computeRay) {
    Ray r = Ray(Point(2,3,4) , Vector(1,0,0) );
    EXPECT_EQ(r.pos(0) , Point(2,3,4) );
    EXPECT_EQ(r.pos(1) , Point(3,3,4) );
    EXPECT_EQ(r.pos(-1), Point(1,3,4) );
    EXPECT_EQ(r.pos(2.5), Point(4.5,3,4));
}

TEST(RaySphereTest, intersectTwice) {
    Ray r = Ray(Point(0,0,-5), Vector(0,0,1));
    auto s = Sphere::make();
    Iset iset;
    s->intersect(r,iset);
    EXPECT_FLOAT_EQ(iset.begin()->t, 4.0);
    EXPECT_FLOAT_EQ((++iset.begin())->t, 6.0);
}

TEST(RaySphereTest, intersectTangent) {
    Ray r = Ray(Point(0,1,-5) , Vector(0,0,1));
    auto s = Sphere::make();
    Iset iset;
    s->intersect(r,iset);
    EXPECT_FLOAT_EQ(iset.begin()->t, (++iset.begin())->t);
    EXPECT_FLOAT_EQ(iset.begin()->t, 5.0);
}

TEST(RaySphereTest, missSphere) {
    Ray r = Ray(Point(0,2,-5) , Vector(0,0,1));
    auto s = Sphere::make();
    Iset iset;
    EXPECT_FALSE(s->intersect(r, iset));
}

TEST(RaySphereTest, insideSphere) {
    Ray r = Ray(Point(0,0,0), Vector(0,0,1));
    auto s = Sphere::make();
    Iset iset;
    s->intersect(r,iset);
    EXPECT_FLOAT_EQ(iset.begin()->t, -1.0);
    EXPECT_FLOAT_EQ((++iset.begin())->t, 1.0);
}

TEST(RaySphereTest, sphereBehindRay) {
    Ray r(Point(0,0,5), Vector(0,0,1));
    auto s = Sphere::make();
    Iset iset;
    s->intersect(r,iset);
    EXPECT_FLOAT_EQ(iset.begin()->t, -6.0);
    EXPECT_FLOAT_EQ((++iset.begin())->t, -4.0);
    EXPECT_EQ(s, iset.begin()->obj);
}

TEST(RaySphereTest, findHitPositiveT) {
    auto s = Sphere::make();
    Intersection i1 = Intersection(1,s);
    Intersection i2 = Intersection(2,s);
    Iset iset{i1, i2};
    EXPECT_EQ(i1, hit(iset));
}

TEST(RaySphereTest, findHitSomeNegativeT) {
    auto s = Sphere::make();
    Intersection i1 = Intersection(-1,s);
    Intersection i2 = Intersection(1,s);
    Iset iset{i1, i2};
    EXPECT_EQ(i2, hit(iset));
}

TEST(RaySphereTest, findHitAllNegativeT) {
    auto s = Sphere::make();
    Intersection i1 = Intersection(-2,s);
    Intersection i2 = Intersection(-1,s);
    Iset iset{i1, i2};
    EXPECT_TRUE(hit(iset).isEmpty());
}

TEST(RaySphereTest, findHitRandom) {
    auto s = Sphere::make();
    Intersection i1 = Intersection(5,s);
    Intersection i2 = Intersection(7,s);
    Intersection i3 = Intersection(-3,s);
    Intersection i4 = Intersection(2,s);
    Iset iset{i1,i2,i3,i4};
    EXPECT_EQ(hit(iset), i4);
}

TEST(RaySphereTest, rayTranslate) {
    Ray r(Point(1,2,3), Vector(0,1,0) );
    Matrix M = Matrix::translation(3,4,5);
    Ray r2 = r.transform(M);
    EXPECT_EQ(r2.origin, Point(4,6,8) );
    EXPECT_EQ(r2.dir, Vector(0,1,0));
}

TEST(RaySphereTest, rayScale) {
    Ray r(Point(1,2,3) , Vector(0,1,0));
    Matrix M = Matrix::scaling(2,3,4);
    Ray r2 = r.transform(M);
    EXPECT_EQ(r2.origin, Point(2,6,12));
    EXPECT_EQ(r2.dir, Vector(0,3,0));
}

TEST(RaySphereTest, sphereTransform) {
    auto s = Sphere::make();
    EXPECT_EQ(s->getTransform(), Matrix::identity(4));
    Matrix T = Matrix::translation(2,3,4);
    s->setTransform(T);
    EXPECT_EQ(s->getTransform(), T);
}

TEST(RaySphereTest, intersectScaledSphere) {
    Ray r = Ray(Point(0,0,-5) , Vector(0,0,1));
    auto s = Sphere::make();
    s->setTransform(Matrix::scaling(2,2,2));
    Iset iset;
    EXPECT_TRUE(s->intersect(r, iset));
    EXPECT_FLOAT_EQ(iset.begin()->t, 3);
    EXPECT_FLOAT_EQ((++iset.begin())->t, 7);
}

TEST(RaySphereTest, intersectTranslatedSphere) {
    Ray r = Ray(Point(0,0,-5), Vector(0,0,1));
    auto s = Sphere::make();
    s->setTransform(Matrix::translation(5,0,0));
    Iset iset;
    EXPECT_FALSE(s->intersect(r,iset));
    EXPECT_TRUE(iset.empty());
}