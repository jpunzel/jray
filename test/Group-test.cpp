#include "gtest/gtest.h"
#include "Tuple.h"
#include "Vector.h"
#include "Point.h"
#include "Ray.h"
#include "Sphere.h"
#include "Group.h"
#include <iostream>
#include <memory>

TEST(GroupTest, intersectEmptyGroup) {
    auto g = Group::make();

    Ray r = Ray(Point(0,0,0), Vector(0,0,1));
    Iset xs;
    bool hit = g->localIntersect(r, xs);
    EXPECT_FALSE(hit);
    EXPECT_EQ(xs.size(), 0);
}

TEST(GroupTest, intersectNonemptyGroup) {
    auto g = Group::make();
    auto s1 = Sphere::make();
    auto s2 = Sphere::make();
    s2->setTransform(Matrix::translation(0,0,-3));
    auto s3 = Sphere::make();
    s3->setTransform(Matrix::translation(5,0,0));
    g->addChild(s1);
    g->addChild(s2);
    g->addChild(s3);
    Ray r = Ray(Point(0,0,-5), Vector(0,0,1));
    Iset xs;
    bool hit = g->localIntersect(r, xs);
    EXPECT_TRUE(hit);
    EXPECT_EQ(xs.size(), 4);
    auto it = xs.begin();
    EXPECT_EQ(it->obj, s2);
    EXPECT_EQ(it->obj->getParent(), g);
    EXPECT_EQ((++it)->obj, s2);
    EXPECT_EQ((++it)->obj, s1);
    EXPECT_EQ((++it)->obj, s1);
}

TEST(GroupTest, intersectTransformedGroup) {
    auto g = Group::make();
    g->setTransform(Matrix::scaling(2,2,2));
    auto s = Sphere::make();
    s->setTransform(Matrix::translation(5,0,0));
    g->addChild(s);
    Ray r = Ray(Point(10,0,-10), Vector(0,0,1));
    Iset xs;
    g->intersect(r, xs);
    EXPECT_EQ(xs.size(), 2);
}

TEST(GroupTest, worldToObject) {
    auto g1 = Group::make();
    auto g2 = Group::make();
    g1->setTransform(Matrix::rotation_y(PI/2));
    g2->setTransform(Matrix::scaling(2,2,2));
    g1->addChild(g2);
    auto s = Sphere::make();
    s->setTransform(Matrix::translation(5,0,0));
    g2->addChild(s);
    Point p = s->world_to_object(Point(-2,0,-10));
    EXPECT_EQ(p, Point(0,0,-1));
}

TEST(GroupTest, normalObjectToWorld) {
    auto g1 = Group::make();
    auto g2 = Group::make();
    g1->setTransform(Matrix::rotation_y(PI/2));
    g2->setTransform(Matrix::scaling(1,2,3));
    g1->addChild(g2);
    auto s = Sphere::make();
    s->setTransform(Matrix::translation(5,0,0));
    g2->addChild(s);
    Vector n = s->normal_to_world(Vector(sqrt(3)/3, sqrt(3)/3, sqrt(3)/3));
    EXPECT_EQ(n, Vector(0.285714, 0.428571, -0.857143));
}

TEST(GroupTest, normalOnChildObject) {
    auto g1 = Group::make();
    auto g2 = Group::make();
    g1->setTransform(Matrix::rotation_y(PI/2));
    g2->setTransform(Matrix::scaling(1,2,3));
    g1->addChild(g2);
    auto s = Sphere::make();
    s->setTransform(Matrix::translation(5,0,0));
    g2->addChild(s);
    Vector n = s->normalAt(Point(1.7321, 1.1547, -5.5774));
    EXPECT_EQ(n, Vector(0.285704, 0.428543, -0.857161));
}