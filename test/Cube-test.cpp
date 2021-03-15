#include "gtest/gtest.h"
#include "Ray.h"
#include "Cube.h"
#include <iostream>
#include <memory>

TEST(CubeTest, rayIntersectsCube) {
    auto c = Cube::make();

    // Try to intersect all 6 sides from the same distance
    Ray r1 = Ray(Point(5,0.5,0), Vector(-1,0,0));
    Ray r2 = Ray(Point(-5,0.5,0), Vector(1,0,0));
    Ray r3 = Ray(Point(0.5,5,0), Vector(0,-1,0));
    Ray r4 = Ray(Point(0.5,-5,0), Vector(0,1,0));
    Ray r5 = Ray(Point(0.5,0,5), Vector(0,0,-1));
    Ray r6 = Ray(Point(0.5,0,-5), Vector(0,0,1));
    // Intersect from inside
    Ray r7 = Ray(Point(0,0.5,0), Vector(0,0,1)); 

    for (auto r : {r1,r2,r3,r4,r5,r6} ) {
        Iset xs;
        bool hit = c->localIntersect(r, xs);
        EXPECT_TRUE(hit);
        EXPECT_EQ(xs.size(), 2);
        EXPECT_FLOAT_EQ(xs.begin()->t, 4);
        EXPECT_FLOAT_EQ((++xs.begin())->t, 6);
    }

    Iset xs;
    bool hit = c->localIntersect(r7, xs);
    EXPECT_TRUE(hit);
    EXPECT_EQ(xs.size(), 2);
    EXPECT_FLOAT_EQ(xs.begin()->t, -1);
    EXPECT_FLOAT_EQ((++xs.begin())->t, 1);

}

TEST(CubeTest, rayMissesCube) {
    auto c = Cube::make();

    Ray r1 = Ray(Point(-2,0,0), Vector(0.2673,0.5345,0.8018));
    Ray r2 = Ray(Point(0,-2,0), Vector(0.8018,0.2673,0.5345));
    Ray r3 = Ray(Point(0,0,-2), Vector(0.5345,0.8018,0.2673));
    Ray r4 = Ray(Point(2,0,2), Vector(0,0,-1));
    Ray r5 = Ray(Point(0,2,2), Vector(0,-1,0));
    Ray r6 = Ray(Point(2,2,0), Vector(-1,0,0));

    for (auto r : {r1,r2,r3,r4,r5,r6} ) {
        Iset xs;
        bool hit = c->localIntersect(r,xs);
        EXPECT_FALSE(hit);
        EXPECT_EQ(xs.size(), 0);
    }
}

TEST(CubeTest, normalAtCube) {
    auto c = Cube::make();

    std::vector<Vector> expected_normals = { Vector(1,0,0),
                                             Vector(-1,0,0),
                                             Vector(0,1,0),
                                             Vector(0,-1,0),
                                             Vector(0,0,1),
                                             Vector(0,0,-1),
                                             Vector(1,0,0),
                                             Vector(-1,0,0)
                                           };

    int i = 0;
    for ( auto p : { Point(1, 0.5, -0.8),
                     Point(-1, -0.2, 0.9),
                     Point(-0.4, 1, -0.1),
                     Point(0.3, -1, -0.7),
                     Point(-0.6, 0.3, 1),
                     Point(0.4, 0.4, -1),
                     Point(1,1,1),
                     Point(-1,-1,-1) } ) { 
    
        EXPECT_EQ(c->localNormalAt(p), expected_normals[i++]);
    }

}