#include "gtest/gtest.h"
#include "Tuple.h"
#include "Vector.h"
#include "Point.h"
#include "Ray.h"
#include "Sphere.h"
#include "Cube.h"
#include "CSG.h"
#include <iostream>
#include <memory>

TEST(CSGTest, createCSG) {
    auto s1 = Sphere::make();
    auto s2 = Cube::make();
    auto csg = CSG::make(CSG_UNION, s1, s2);
    EXPECT_EQ(csg->op, CSG_UNION);
    EXPECT_EQ(csg->left, s1);
    EXPECT_EQ(csg->right, s2);
    EXPECT_EQ(s1->getParent(), csg);
    EXPECT_EQ(s2->getParent(), csg);
}