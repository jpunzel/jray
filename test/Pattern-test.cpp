#include "gtest/gtest.h"
#include "Tuple.h"
#include "Vector.h"
#include "Point.h"
#include "Ray.h"
#include "Pattern.h"
#include "Plane.h"
#include "Sphere.h"
#include <iostream>
#include <memory>


TEST(PatternTest, stripePattern) {
    auto p = StripePattern::make(Color::White, Color::Black);
    EXPECT_EQ( p->patternAt( Point(0,0,0)    ), Color::White );
    EXPECT_EQ( p->patternAt( Point(0,1,0)    ), Color::White );
    EXPECT_EQ( p->patternAt( Point(0,2,0)    ), Color::White );
    EXPECT_EQ( p->patternAt( Point(0,0,1)    ), Color::White );
    EXPECT_EQ( p->patternAt( Point(0,0,2)    ), Color::White );
    EXPECT_EQ( p->patternAt( Point(0.9,0,0)  ), Color::White );
    EXPECT_EQ( p->patternAt( Point(1,0,0)    ), Color::Black );
    EXPECT_EQ( p->patternAt( Point(-0.1,0,0) ), Color::Black );
    EXPECT_EQ( p->patternAt( Point(-1,0,0)   ), Color::Black );
    EXPECT_EQ( p->patternAt( Point(-1.1,0,0) ), Color::White );
}

TEST(PatternTest, lightingWithPattern) {
    Material m;
    auto p = StripePattern::make(Color::White, Color::Black);
    auto s = Sphere::make();

    m.setPattern(p);
    m.setAmbient(1);
    m.setDiffuse(0);
    m.setSpecular(0);
    Vector eyev = Vector(0,0,-1);
    Vector normalv = Vector(0,0,-1);
    Light light = Light(Point(0,0,-10), Color::White);
    Color c1 = m.lighting(s, light, Point(0.9,0,0), eyev, normalv, false);
    Color c2 = m.lighting(s, light, Point(1.1,0,0), eyev, normalv, false);
    EXPECT_EQ(c1, Color::White);
    EXPECT_EQ(c2, Color::Black);

}