#include "gtest/gtest.h"
#include "Ray.h"
#include "Cylinder.h"
#include "Cone.h"
#include <iostream>
#include <memory>

TEST(CylinderTest, rayMissesCylinder) {
    auto c = Cylinder::make();
    Ray r1 = Ray(Point(1,0,0), normalize(Vector(0,1,0)) );
    Ray r2 = Ray(Point(0,0,0), normalize(Vector(0,1,0)) );
    Ray r3 = Ray(Point(0,0,-5), normalize(Vector(1,1,1)) );
    for (auto r : {r1,r2,r3} ) {
        Iset xs;
        bool hit = c->localIntersect(r, xs);
        EXPECT_EQ(xs.size(), 0);
        EXPECT_FALSE(hit);
    }
}

TEST(CylinderTest, rayHitsCylinder) {
    auto c = Cylinder::make();
    std::vector<std::pair<double,double>> expected_t = { {5,5}, {4,6}, {6.80798, 7.0887237} };
    Ray r1 = Ray(Point(1,0,-5), normalize(Vector(0,0,1)) );
    Ray r2 = Ray(Point(0,0,-5), normalize(Vector(0,0,1)) );
    Ray r3 = Ray(Point(0.5,0,-5) , normalize(Vector(0.1,1,1)) );
    int i = 0;
    for (auto r : {r1,r2,r3} ) {
        Iset xs;
        bool hit = c->localIntersect(r,xs);
        
        EXPECT_EQ(xs.size(), 2);
        EXPECT_TRUE(hit);
        EXPECT_FLOAT_EQ( expected_t[i].first, xs.begin()->t );
        EXPECT_FLOAT_EQ( expected_t[i].second, (++xs.begin())->t );
        i++;
    }
}

TEST(CylinderTest, normalVector) {
    auto c = Cylinder::make();
    std::vector<Vector> expected_normals = { Vector(1,0,0), Vector(0,0,-1), Vector(0,0,1), Vector(-1,0,0) };
    int i = 0;
    for ( auto p : { Point(1,0,0), Point(0,5,-1), Point(0,-2,1), Point(-1,1,0) }) {
        EXPECT_EQ(expected_normals[i], c->localNormalAt(p) );
        i++;
    }
}

TEST(CylinderTest, truncatedCylinder) {
    auto c = Cylinder::make(1,2);

    Ray r1 = Ray(Point(0,1.5,0), normalize(Vector(0.1,1,0)) );
    Ray r2 = Ray(Point(0,3,-5), normalize(Vector(0,0,1)) );
    Ray r3 = Ray(Point(0,0,-5) , normalize(Vector(0,0,1)) );
    Ray r4 = Ray(Point(0,2,-5) , normalize(Vector(0,0,1)) );
    Ray r5 = Ray(Point(0,1,-5) , normalize(Vector(0,0,1)) );
    Ray r6 = Ray(Point(0,1.5,-2) , normalize(Vector(0,0,1)) );
    std::vector<int> expected_counts = {0,0,0,0,0,2};
    int i = 0;
    for (auto r : {r1,r2,r3,r4,r5,r6} ) {
        Iset xs;
        bool hit = c->localIntersect(r,xs);
        
        EXPECT_EQ(xs.size(), expected_counts[i]);
        i++;
    }
}

TEST(CylinderTest, cappedCylinder) {
    auto c = Cylinder::make(1,2,true);
    Ray r1 = Ray(Point(0,3,0)   , normalize(Vector(0,-1,0) ) );
    Ray r2 = Ray(Point(0,3,-2)  , normalize(Vector(0,-1,2) ) );
    Ray r3 = Ray(Point(0,4,-2)  , normalize(Vector(0,-1,1) ) );
    Ray r4 = Ray(Point(0,0,-2)  , normalize(Vector(0,1,2) ) );
    Ray r5 = Ray(Point(0,-1,-2) , normalize(Vector(0,1,1) ) );

    for (auto r : {r1,r2,r3,r4,r5} ) {
        Iset xs;
        bool hit = c->localIntersect(r,xs);

        EXPECT_TRUE(hit);
        EXPECT_EQ(xs.size(), 2);
    }
}

TEST(CylinderTest, normalAtCaps) {
    auto c = Cylinder::make(1,2,true);
    std::vector<Vector> expected_normals = { Vector(0,-1,0),
                                             Vector(0,-1,0),
                                             Vector(0,-1,0),
                                             Vector(0,1,0),
                                             Vector(0,1,0),
                                             Vector(0,1,0) };
    int i = 0;
    for ( auto p : { Point(0,1,0),
                     Point(0.5,1,0),
                     Point(0,1,0.5),
                     Point(0,2,0),
                     Point(0.5,2,0),
                     Point(0,2,0.5) }) {
        EXPECT_EQ(expected_normals[i], c->localNormalAt(p) );
        i++;
    }
}

TEST(CylinderTest, coneIntersect) {
    auto c = Cone::make();
    std::vector<std::pair<double,double>> expected_t = { {5,5}, 
                                                       {8.6602545, 8.6602545},
                                                       {4.5500555, 49.44994} };
    Ray r1 = Ray(Point(0,0,-5), normalize(Vector(0,0,1)) );
    Ray r2 = Ray(Point(0,0,-5), normalize(Vector(1,1,1)) );
    Ray r3 = Ray(Point(1,1,-5) , normalize(Vector(-0.5,-1,1)) );
    int i = 0;
    for (auto r : {r1,r2,r3} ) {
        Iset xs;
        bool hit = c->localIntersect(r,xs);

        EXPECT_FALSE(std::static_pointer_cast<Cone>(c)->closed());
        EXPECT_EQ(xs.size(), 2);
        EXPECT_TRUE(hit);
        EXPECT_FLOAT_EQ( expected_t[i].first, xs.begin()->t );
        EXPECT_FLOAT_EQ( expected_t[i].second, (++xs.begin())->t );
        i++;
    }
}

TEST(CylinderTest, coneIntersectParallelToHalf) {
    auto c = Cone::make();
    Ray r = Ray(Point(0,0,-1), normalize(Vector(0,1,1)) );
    Iset xs;
    bool hit = c->localIntersect(r, xs);
    EXPECT_EQ(xs.size(), 1);
    EXPECT_TRUE(hit);
    EXPECT_FLOAT_EQ( xs.begin()->t, 0.35355338 );
}

TEST(CylinderTest, coneIntersectCaps) {

    auto c = Cone::make(-0.5, 0.5, true);

    Ray r1 = Ray(Point(0,0,-5), normalize(Vector(0,1,0)) );
    Ray r2 = Ray(Point(0,0,-0.25), normalize(Vector(0,1,1)) );
    Ray r3 = Ray(Point(0,0,-0.25) , normalize(Vector(0,1,0)) );
    std::vector<int> expected_counts = {0,2,4};
    int i = 0;
    for (auto r : {r1,r2,r3} ) {
        Iset xs;
        bool hit = c->localIntersect(r,xs);
        
        EXPECT_EQ(xs.size(), expected_counts[i]);
        i++;
    }
}

TEST(CylinderTest, coneNormal) {
    auto c = Cone::make();
    std::vector<Vector> expected_normals = { Vector(0,0,0), Vector(1,-sqrt(2),1), Vector(-1,1,0) };
    int i = 0;
    for ( auto p : { Point(0,0,0), Point(1,1,1), Point(-1,-1,0) }) {
        EXPECT_EQ(expected_normals[i], c->localNormalAt(p) );
        i++;
    }
}