#include "gtest/gtest.h"
#include "Ray.h"
#include "Triangle.h"
#include "ObjParser.h"
#include <iostream>
#include <memory>

TEST(TriangleTest, constructTriangle) {
    Point p1 = Point(0,1,0);
    Point p2 = Point(-1,0,0);
    Point p3 = Point(1,0,0);
    auto t = std::static_pointer_cast<Triangle>(Triangle::make(p1,p2,p3));
    EXPECT_EQ(t->p1, p1);
    EXPECT_EQ(t->p2, p2);
    EXPECT_EQ(t->p3, p3);
    EXPECT_EQ(t->e1, Vector(-1,-1,0));
    EXPECT_EQ(t->e2, Vector(1,-1,0));
    EXPECT_EQ(t->n1, Vector(0,0,-1));
    EXPECT_EQ(t->localNormalAt(Point(0,0.5,0)), t->n1);
    EXPECT_EQ(t->localNormalAt(Point(-0.5,0.75,0)), t->n1);
    EXPECT_EQ(t->localNormalAt(Point(0.5,0.25,0)), t->n1);
}

TEST(TriangleTest, intersectRayParallel) {
    auto t = Triangle::make(Point(0,1,0), Point(-1,0,0), Point(1,0,0));
    Ray r = Ray(Point(0,-1,-2), Vector(0,1,0));
    Iset xs;
    bool hit = t->localIntersect(r, xs);
    EXPECT_FALSE(hit);
    EXPECT_EQ(xs.size(), 0); // we should miss
}

TEST(TriangleTest, rayMissesEdgeP1P3) {
    auto t = Triangle::make(Point(0,1,0), Point(-1,0,0), Point(1,0,0));
    Ray r = Ray(Point(1,1,-2), Vector(0,0,1));
    Iset xs;
    bool hit = t->localIntersect(r, xs);
    EXPECT_FALSE(hit);
    EXPECT_EQ(xs.size(), 0);
}

TEST(TriangleTest, rayMissesEdgeP1P2) {
    auto t = Triangle::make(Point(0,1,0), Point(-1,0,0), Point(1,0,0));
    Ray r = Ray(Point(-1,1,-2), Vector(0,0,1));
    Iset xs;
    bool hit = t->localIntersect(r, xs);
    EXPECT_FALSE(hit);
    EXPECT_EQ(xs.size(), 0);
}

TEST(TriangleTest, rayMissesEdgeP2P3) {
    auto t = Triangle::make(Point(0,1,0), Point(-1,0,0), Point(1,0,0));
    Ray r = Ray(Point(0,-1,-2), Vector(0,0,1));
    Iset xs;
    bool hit = t->localIntersect(r, xs);
    EXPECT_FALSE(hit);
    EXPECT_EQ(xs.size(), 0);
}

TEST(TriangleTest, rayHitsTriangle) {
    auto t = Triangle::make(Point(0,1,0), Point(-1,0,0), Point(1,0,0));
    Ray r = Ray(Point(0,0.5,-2), Vector(0,0,1));
    Iset xs;
    bool hit = t->localIntersect(r, xs);
    EXPECT_TRUE(hit);
    EXPECT_EQ(xs.size(), 1);
    EXPECT_FLOAT_EQ(xs.begin()->t, 2);
}

TEST(TriangleTest, objVertexData) {
    // Since there's not really a quick portable way to make a temp file
    // or get the relative path of a test obj file,  we'll just use ObjParser's
    // stream constructor and pass it a stringstream made from a heredoc
    std::string testfile =
    R"EOF(
        v -1 1 0
        v -1 0.5 0
        v 1 0 0
        v 1 1 0
    )EOF";
    std::istringstream iss(testfile);
    ObjParser parser = ObjParser(iss);
    EXPECT_EQ(parser.vertices[0], Point(-1,1,0));
    EXPECT_EQ(parser.vertices[1], Point(-1,0.5,0));
    EXPECT_EQ(parser.vertices[2], Point(1,0,0));
    EXPECT_EQ(parser.vertices[3], Point(1,1,0));
}

TEST(TriangleTest, objTriangleData) {
    std::string testfile =
    R"EOF(
        v -1 1 0
        v -1 0 0
        v 1 0 0
        v 1 1 0
        f 1 2 3
        f 1 3 4
    )EOF";
    std::istringstream iss(testfile);
    ObjParser parser = ObjParser(iss);
    EXPECT_EQ(std::static_pointer_cast<Triangle>(parser.obj->getChildren()[0])->p1, parser.vertices[0]);
    EXPECT_EQ(std::static_pointer_cast<Triangle>(parser.obj->getChildren()[0])->p2, parser.vertices[1]);
    EXPECT_EQ(std::static_pointer_cast<Triangle>(parser.obj->getChildren()[0])->p3, parser.vertices[2]);
    EXPECT_EQ(std::static_pointer_cast<Triangle>(parser.obj->getChildren()[1])->p1, parser.vertices[0]);
    EXPECT_EQ(std::static_pointer_cast<Triangle>(parser.obj->getChildren()[1])->p2, parser.vertices[2]);
    EXPECT_EQ(std::static_pointer_cast<Triangle>(parser.obj->getChildren()[1])->p3, parser.vertices[3]);
}

TEST(TriangleTest, objPolygonData) {
    std::string testfile =
    R"EOF(
        v -1 1 0
        v -1 0 0
        v 1 0 0
        v 1 1 0
        v 0 2 0
        f 1 2 3 4 5
    )EOF";
    std::istringstream iss(testfile);
    ObjParser parser = ObjParser(iss);
    EXPECT_EQ(std::static_pointer_cast<Triangle>(parser.obj->getChildren()[0])->p1, parser.vertices[0]);
    EXPECT_EQ(std::static_pointer_cast<Triangle>(parser.obj->getChildren()[0])->p2, parser.vertices[1]);
    EXPECT_EQ(std::static_pointer_cast<Triangle>(parser.obj->getChildren()[0])->p3, parser.vertices[2]);
    EXPECT_EQ(std::static_pointer_cast<Triangle>(parser.obj->getChildren()[1])->p1, parser.vertices[0]);
    EXPECT_EQ(std::static_pointer_cast<Triangle>(parser.obj->getChildren()[1])->p2, parser.vertices[2]);
    EXPECT_EQ(std::static_pointer_cast<Triangle>(parser.obj->getChildren()[1])->p3, parser.vertices[3]);
    EXPECT_EQ(std::static_pointer_cast<Triangle>(parser.obj->getChildren()[2])->p1, parser.vertices[0]);
    EXPECT_EQ(std::static_pointer_cast<Triangle>(parser.obj->getChildren()[2])->p2, parser.vertices[3]);
    EXPECT_EQ(std::static_pointer_cast<Triangle>(parser.obj->getChildren()[2])->p3, parser.vertices[4]);
}