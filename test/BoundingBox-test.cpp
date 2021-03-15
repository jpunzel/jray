#include "gtest/gtest.h"
#include "Ray.h"
#include "BoundingBox.h"
#include "Sphere.h"
#include "Cylinder.h"
#include "TestShape.h"
#include "Group.h"
#include <iostream>
#include <memory>

TEST(BoundingBoxTest, createEmptyBoundingBox) {
    BoundingBox bb = BoundingBox();
    EXPECT_EQ(bb.min, BoundingBox::infinityPoint);
    EXPECT_EQ(bb.max, BoundingBox::minusInfinityPoint);
}
TEST(BoundingBoxTest, createBoundingBoxWithVolume) {
    BoundingBox bb = BoundingBox(Point(-1,-2,-3), Point(3,2,1));
    EXPECT_EQ(bb.min, Point(-1,-2,-3));
    EXPECT_EQ(bb.max, Point(3,2,1));
}
TEST(BoundingBoxTest, addPointsToBoundingBox) {
    BoundingBox bb = BoundingBox();
    bb.add(Point(-5,2,0));
    bb.add(Point(7,0,-3));
    EXPECT_EQ(bb.min, Point(-5,0,-3));
    EXPECT_EQ(bb.max, Point(7,2,0));
}
TEST(BoundingBoxTest, addBoundingBoxes) {
    BoundingBox box1 = BoundingBox(Point(-5,-2,0), Point(7,4,4));
    BoundingBox box2 = BoundingBox(Point(8,-7,-2), Point(14,2,8));
    box1.add(box2);
    EXPECT_EQ(box1.min, Point(-5,-7,-2));
    EXPECT_EQ(box1.max, Point(14,4,8));
}
TEST(BoundingBoxTest, checkBoxContainsPoint) {
    BoundingBox box = BoundingBox(Point(5,-2,0), Point(11,4,7));
    std::vector<bool> expected_results = { true, true, true,
                                           false, false, false, false, false, false };
    int i = 0;
    for ( auto p : { Point(5,-2,0),
                     Point(11,4,7),
                     Point(8,1,3),
                     Point(3,0,3),
                     Point(8,-4,3),
                     Point(8,1,-1),
                     Point(13,1,3),
                     Point(8,5,3),
                     Point(8,1,8) }) {
        EXPECT_EQ(expected_results[i], box.contains(p));
        i++;
    }
}

TEST(BoundingBoxTest, checkBoxContainsBox) {
    BoundingBox box = BoundingBox(Point(5,-2,0), Point(11,4,7));
    std::vector<bool> expected_results = { true, true,
                                           false, false };
    int i = 0;
    std::vector<Point> maxes = { Point(11,4,7), Point(10,3,6), Point(10,3,6), Point(12,5,8) };
    for ( auto min : { Point(5,-2,0),
                     Point(6,-1,1),
                     Point(4,-3,-1),
                     Point(6,-1,1)
                     }) {
        EXPECT_EQ(expected_results[i], box.contains(BoundingBox(min, maxes[i])));
        i++;
    }
}

TEST(BoundingBoxTest, transformBoundingBox) {
    BoundingBox box = BoundingBox(Point(-1,-1,-1), Point(1,1,1));
    Matrix M = Matrix::rotation_x(PI/4) * Matrix::rotation_y(PI/4);
    BoundingBox box2 = box.transform(M);
    EXPECT_EQ(box2.min, Point(-1.41421, -1.70711, -1.70711));
    EXPECT_EQ(box2.max, Point(1.41421, 1.70711, 1.70711));
}

TEST(BoundingBoxTest, getShapeBoundsInParentSpace) {
    auto s = Sphere::make();
    s->setTransform(Matrix::translation(1,-3,5) * Matrix::scaling(0.5,2,4));
    BoundingBox box = s->parentBounds();
    EXPECT_EQ(box.min, Point(0.5,-5,1));
    EXPECT_EQ(box.max, Point(1.5,-1,9));
}

TEST(BoundingBoxTest, groupBoundingBoxContainsChildren) {
    auto s = Sphere::make();
    s->setTransform(Matrix::translation(2,5,-3) * Matrix::scaling(2,2,2));
    auto c = Cylinder::make(-2,2);
    c->setTransform(Matrix::translation(-4,-1,4) * Matrix::scaling(0.5,1,0.5));
    auto g = Group::make();
    g->addChild(s);
    g->addChild(c);
    BoundingBox box = g->bounds();
    EXPECT_EQ(box.min, Point(-4.5,-3,-5));
    EXPECT_EQ(box.max, Point(4,7,4.5));
}

TEST(BoundingBoxTest, intersectRayWithBoundingBoxAtOrigin) {
    BoundingBox box = BoundingBox(Point(-1,-1,-1), Point(1,1,1));

    std::vector<bool> expected_results = { true, true, true, true, true, true, true,
                                           false, false, false, false, false, false };
    int i = 0;
    std::vector<Point> origins = { Point(5,0.5,0),
                                   Point(-5,0.5,0),
                                   Point(0.5,5,0),
                                   Point(0.5,-5,0),
                                   Point(0.5,0,5),
                                   Point(0.5,0,-5),
                                   Point(0,0.5,0),
                                   Point(-2,0,0),
                                   Point(0,-2,0),
                                   Point(0,0,-2),
                                   Point(2,0,2),
                                   Point(0,2,2),
                                   Point(2,2,0) 
                                 };
    std::vector<Vector> directions = { Vector(-1,0,0),
                                       Vector(1,0,0),
                                       Vector(0,-1,0),
                                       Vector(0,1,0),
                                       Vector(0,0,-1),
                                       Vector(0,0,1),
                                       Vector(0,0,1),
                                       Vector(2,4,6),
                                       Vector(6,2,4),
                                       Vector(4,6,2),
                                       Vector(0,0,-1),
                                       Vector(0,-1,0),
                                       Vector(-1,0,0)
                                     };
    for ( auto o : origins ) {
        Ray r = Ray(o, normalize(directions[i]));
        EXPECT_EQ(expected_results[i], box.intersects(r));
        i++;
    }
}

TEST(BoundingBoxTest, intersectRayWithNonCubicBoundingBox) {
    BoundingBox box = BoundingBox(Point(5,-2,0), Point(11,4,7));

    std::vector<bool> expected_results = { true, true, true, true, true, true, true,
                                           false, false, false, false, false, false };
    int i = 0;
    std::vector<Point> origins = { Point(15,1,2),
                                   Point(-5,-1,4),
                                   Point(7,6,5),
                                   Point(9,-5,6),
                                   Point(8,2,12),
                                   Point(6,0,-5),
                                   Point(8,1,3.5),
                                   Point(9,-1,-8),
                                   Point(8,3,-4),
                                   Point(9,-1,-2),
                                   Point(4,0,9),
                                   Point(8,6,-1),
                                   Point(12,5,4)
                                 };
    std::vector<Vector> directions = { Vector(-1,0,0),
                                       Vector(1,0,0),
                                       Vector(0,-1,0),
                                       Vector(0,1,0),
                                       Vector(0,0,-1),
                                       Vector(0,0,1),
                                       Vector(0,0,1),
                                       Vector(2,4,6),
                                       Vector(6,2,4),
                                       Vector(4,6,2),
                                       Vector(0,0,-1),
                                       Vector(0,-1,0),
                                       Vector(-1,0,0)
                                     };
    for ( auto o : origins ) {
        Ray r = Ray(o, normalize(directions[i]));
        EXPECT_EQ(expected_results[i], box.intersects(r));
        i++;
    }
}

TEST(BoundingBoxTest, intersectGroupDoesNotTestChildrenIfBoxMissed) {
    auto child = TestShape::make();
    auto shape = Group::make();
    shape->addChild(child);
    Ray r = Ray(Point(0,0,-5), Vector(0,1,0));
    Iset xs;
    bool hit = shape->localIntersect(r, xs);
    EXPECT_EQ( std::static_pointer_cast<TestShape>(child)->saved_ray, nullptr);
}

TEST(BoundingBoxTest, intersectGroupTestsChildrenIfBoxHit) {
    auto child = TestShape::make();
    auto shape = Group::make();
    shape->addChild(child);
    Ray r = Ray(Point(0,0,-5), Vector(0,0,1));
    Iset xs;
    bool hit = shape->localIntersect(r, xs);
    EXPECT_TRUE(hit);
    EXPECT_EQ( std::static_pointer_cast<TestShape>(child)->saved_ray->origin, r.origin );
    EXPECT_EQ( std::static_pointer_cast<TestShape>(child)->saved_ray->dir, r.dir );
}

TEST(BoundingBoxTest, splittingPerfectCube) {
    BoundingBox box = BoundingBox(Point(-1,-4,-5), Point(9,6,5));
    std::pair<BoundingBox,BoundingBox> split = box.splitBounds();
    auto left = std::get<0>(split);
    auto right = std::get<1>(split);
    EXPECT_EQ(left.min, Point(-1,-4,-5));
    EXPECT_EQ(left.max, Point(4,6,5));
    EXPECT_EQ(right.min, Point(4,-4,-5));
    EXPECT_EQ(right.max, Point(9,6,5));
}

TEST(BoundingBoxTest, splittingXWideBox) {
    BoundingBox box = BoundingBox(Point(-1,-2,-3), Point(9,5.5,3));
    std::pair<BoundingBox,BoundingBox> split = box.splitBounds();
    auto left = std::get<0>(split);
    auto right = std::get<1>(split);
    EXPECT_EQ(left.min, Point(-1,-2,-3));
    EXPECT_EQ(left.max, Point(4,5.5,3));
    EXPECT_EQ(right.min, Point(4,-2,-3));
    EXPECT_EQ(right.max, Point(9,5.5,3));
}
TEST(BoundingBoxTest, splittingYWideBox) {
    BoundingBox box = BoundingBox(Point(-1,-2,-3), Point(5,8,3));
    std::pair<BoundingBox,BoundingBox> split = box.splitBounds();
    auto left = std::get<0>(split);
    auto right = std::get<1>(split);
    EXPECT_EQ(left.min, Point(-1,-2,-3));
    EXPECT_EQ(left.max, Point(5,3,3));
    EXPECT_EQ(right.min, Point(-1,3,-3));
    EXPECT_EQ(right.max, Point(5,8,3));
}
TEST(BoundingBoxTest, splittingZWideBox) {
    BoundingBox box = BoundingBox(Point(-1,-2,-3), Point(5,3,7));
    std::pair<BoundingBox,BoundingBox> split = box.splitBounds();
    auto left = std::get<0>(split);
    auto right = std::get<1>(split);
    EXPECT_EQ(left.min, Point(-1,-2,-3));
    EXPECT_EQ(left.max, Point(5,3,2));
    EXPECT_EQ(right.min, Point(-1,-2,2));
    EXPECT_EQ(right.max, Point(5,3,7));
}

TEST(BoundingBoxTest, partitionGroupChildren) {
    auto s1 = Sphere::make();
    auto s2 = Sphere::make();
    auto s3 = Sphere::make();
    auto g = Group::make();

    s1->setTransform(Matrix::translation(-2,0,0));
    s2->setTransform(Matrix::translation(2,0,0));
    g->addChild(s1);
    g->addChild(s2);
    g->addChild(s3);
    
    auto pair = g->partitionChildren();
    auto left_children = std::get<0>(pair);
    auto right_children = std::get<1>(pair);
    auto children = g->getChildren();

    EXPECT_EQ(children.size(), 1);
    EXPECT_EQ(children[0], s3);
    EXPECT_EQ(left_children.size(), 1);
    EXPECT_EQ(left_children[0], s1);
    EXPECT_EQ(right_children.size(), 1);
    EXPECT_EQ(right_children[0], s2);
}

TEST(BoundingBoxTest, subdivideGroupPartitionsChildren) {
    auto s1 = Sphere::make();
    auto s2 = Sphere::make();
    auto s3 = Sphere::make();

    s1->setTransform(Matrix::translation(-2,-2,0));
    s2->setTransform(Matrix::translation(-2,2,0) );
    s3->setTransform(Matrix::scaling(4,4,4));
    auto g = Group::make();
    g->addChild(s1);
    g->addChild(s2);
    g->addChild(s3);

    g->divide(1);
    auto children = g->getChildren();
    EXPECT_EQ(children.size(), 2);
    EXPECT_EQ(children[0], s3);
    
    auto subgroup = children[1];
    auto subchildren = std::static_pointer_cast<Group>(subgroup)->getChildren();
    EXPECT_EQ(subchildren.size(), 2);
    auto subsubgroup1 = std::static_pointer_cast<Group>(subchildren[0]);
    auto subsubgroup2 = std::static_pointer_cast<Group>(subchildren[1]);
    EXPECT_EQ(subsubgroup1->getChildren().size(), 1);
    EXPECT_EQ(subsubgroup2->getChildren().size(), 1);
    EXPECT_EQ(subsubgroup1->getChildren()[0], s1);
    EXPECT_EQ(subsubgroup2->getChildren()[0], s2);

}