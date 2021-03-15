#include "gtest/gtest.h"
#include "Tuple.h"
#include "Vector.h"
#include "Point.h"
#include "Ray.h"
#include "Pattern.h"
#include "Plane.h"
#include "Sphere.h"
#include "World.h"
#include <iostream>
#include <memory>

std::shared_ptr<Shape> make_glass_sphere() {
    std::shared_ptr<Shape> glass_sphere = Sphere::make();
    glass_sphere->setMaterialRefractiveIndex(1.5);
    glass_sphere->setMaterialTransparency(1.0);
    return glass_sphere;
}

TEST(ReflectionTest, reflectVector) {
    auto s = Plane::make();
    Ray r = Ray(Point(0,1,-1), Vector(0, -sqrt(2)/2, sqrt(2)/2 ));
    Intersection i = Intersection(sqrt(2)/2, s);
    Icomps comps = i.prepComps(r);
    EXPECT_EQ(comps.reflectv, Vector(0,sqrt(2)/2,sqrt(2)/2));
}

TEST(ReflectionTest, reflectNonReflective) {
    World w;
    w.make_default();
    Ray r = Ray(Point(0,0,0), Vector(0,0,1));
    auto s = w.getShapes()[1];
    s->getMaterial().setAmbient(1.0);
    Intersection i(1, s);

    Icomps comps = i.prepComps(r);
    Iset xs;
    Color reflected_color = w.reflectedColor(comps, xs, 10);
    EXPECT_EQ(reflected_color, Color::Black);
}

TEST(ReflectionTest, reflectReflective) {
    World w;
    w.make_default();

    auto s = Plane::make();
    s->setMaterialReflective(0.5);
    s->setTransform(Matrix::translation(0, -1, 0));
    w.addShape(s);

    Ray r = Ray(Point(0,0,-3), Vector(0,-sqrt(2)/2, sqrt(2)/2));
    Intersection i(sqrt(2), s);

    Icomps comps = i.prepComps(r);
    Iset xs;
    Color reflected = w.reflectedColor(comps, xs, 10);
    //EXPECT_EQ(reflected, Color(0.19032, 0.2379, 0.14274));
    EXPECT_EQ(reflected, Color(0.19033, 0.23792, 0.14275));
}

TEST(ReflectionTest, shadeHitReflection) {
    World w;
    w.make_default();

    auto s = Plane::make();
    s->setMaterialReflective(0.5);
    s->setTransform(Matrix::translation(0, -1, 0));
    w.addShape(s);

    Ray r = Ray(Point(0,0,-3), Vector(0,-sqrt(2)/2, sqrt(2)/2));
    Intersection i(sqrt(2), s);

    Icomps comps = i.prepComps(r);
    Iset xs;
    Color shadehit = w.shadeHit(comps, xs, 10);
    //EXPECT_EQ(shadehit, Color(0.87677, 0.92436, 0.82918));
    EXPECT_EQ(shadehit, Color(0.87676, 0.92434, 0.82917));
}

TEST(ReflectionTest, mutuallyReflectiveMirrors) {
    World w;
    w.addLight( Light(Point(0,0,0), Color::White) );
    auto lower = Plane::make();
    auto upper = Plane::make();

    lower->setMaterialReflective(1.0);
    upper->setMaterialReflective(1.0);
    lower->setTransform(Matrix::translation(0,-1,0));
    upper->setTransform(Matrix::translation(0,1,0));

    w.addShape(upper);
    w.addShape(lower);

    Ray r = Ray(Point(0,0,0), Vector(0,1,0));
    Iset xs;
    EXPECT_EQ( w.colorAt(r,xs), Color::White );
}

TEST(ReflectionTest, findRefractiveIndexOfContainingShapes) {
    auto a = make_glass_sphere();
    a->setTransform(Matrix::scaling(2,2,2));
    a->setMaterialRefractiveIndex(1.5);
    auto b = make_glass_sphere();
    b->setTransform(Matrix::translation(0,0,-0.25));
    b->setMaterialRefractiveIndex(2.0);
    auto c = make_glass_sphere();
    c->setTransform(Matrix::translation(0,0,0.25));
    c->setMaterialRefractiveIndex(2.5);

    Ray r = Ray(Point(0,0,-4), Vector(0,0,1));
    Intersection i1(2, a);
    Intersection i2(2.75, b);
    Intersection i3(3.25, c);
    Intersection i4(4.75, b);
    Intersection i5(5.25, c);
    Intersection i6(6, a);

    Iset xs = { i1, i2, i3, i4, i5, i6 };
    
    Icomps i1comps = i1.prepComps(r, xs);
    Icomps i2comps = i2.prepComps(r, xs);
    Icomps i3comps = i3.prepComps(r, xs);
    Icomps i4comps = i4.prepComps(r, xs);
    Icomps i5comps = i5.prepComps(r, xs);
    Icomps i6comps = i6.prepComps(r, xs);

    EXPECT_EQ( i1comps.rindex_from, 1.0 );
    EXPECT_EQ( i1comps.rindex_to, 1.5 );

    EXPECT_EQ( i2comps.rindex_from, 1.5 );
    EXPECT_EQ( i2comps.rindex_to, 2.0 );

    EXPECT_EQ( i3comps.rindex_from, 2.0 );
    EXPECT_EQ( i3comps.rindex_to, 2.5 );

    EXPECT_EQ( i4comps.rindex_from, 2.5 );
    EXPECT_EQ( i4comps.rindex_to, 2.5 );

    EXPECT_EQ( i5comps.rindex_from, 2.5 );
    EXPECT_EQ( i5comps.rindex_to, 1.5 );

    EXPECT_EQ( i6comps.rindex_from, 1.5 );
    EXPECT_EQ( i6comps.rindex_to, 1.0 );
}

TEST(ReflectionTest, refractedOpaqueSurface) {
    World w;
    w.make_default();
    auto s = w.getShapes()[0];
    Ray r = Ray(Point(0,0,-5), Vector(0,0,1));
    Intersection i1(4,s);
    Intersection i2(6,s);
    Iset xs = { i1, i2 };
    Icomps i1comps = i1.prepComps(r,xs);
    Color c = w.refractedColor(i1comps, xs, 5);
    EXPECT_EQ(c, Color(0,0,0));
}

TEST(ReflectionTest, refractedRecursionLimit) {
    World w;
    w.make_default();
    auto s = w.getShapes()[0];
    s->setMaterialTransparency(1.0);
    s->setMaterialRefractiveIndex(1.5);
    Ray r = Ray(Point(0,0,-5), Vector(0,0,1));
    Intersection i1(4,s);
    Intersection i2(6,s);
    Iset xs = { i1, i2 };
    Icomps i1comps = i1.prepComps(r,xs);
    Color c = w.refractedColor(i1comps, xs, 0);
    EXPECT_EQ(c, Color(0,0,0));
}

TEST(ReflectionTest, refractedTotalInternalReflection) {
    World w;
    w.make_default();
    auto s = w.getShapes()[0];
    s->setMaterialTransparency(1.0);
    s->setMaterialRefractiveIndex(1.5);
    Ray r = Ray(Point(0,0,sqrt(2)/2), Vector(0,1,0));
    Intersection i1(-sqrt(2)/2,s);
    Intersection i2(sqrt(2)/2,s);
    Iset xs = { i1, i2 };
    Icomps i2comps = i2.prepComps(r,xs);
    Color c = w.refractedColor(i2comps, xs, 5);
    EXPECT_EQ(c, Color(0,0,0));
}

TEST(ReflectionTest, refractedColor) {
    World w;
    w.make_default();
    auto A = w.getShapes()[0];
    A->setMaterialAmbient(1.0);
    A->setMaterialPattern(TestPattern::make());
    auto B = w.getShapes()[1];
    B->setMaterialTransparency(1.0);
    B->setMaterialRefractiveIndex(1.5);
    Ray r = Ray(Point(0,0,0.1), Vector(0,1,0));
    Intersection i1(-0.9899, A);
    Intersection i2(-0.4899, B);
    Intersection i3(0.4899, B);
    Intersection i4(0.9899, A);
    Iset xs = { i1, i2, i3, i4 };
    Icomps i3comps = i3.prepComps(r, xs);
    xs.clear();
    Color c = w.refractedColor(i3comps, xs, 5);

    EXPECT_EQ(c, Color(0,0.998875,0.047219));
}

TEST(ReflectionTest, shadeHitRefracted) {
    World w;
    w.make_default();
    auto floor = Plane::make();
    floor->setTransform(Matrix::translation(0,-1,0));
    floor->setMaterialTransparency(0.5);
    floor->setMaterialRefractiveIndex(1.5);
    w.addShape(floor);
    auto ball = Sphere::make();
    ball->setMaterialColor(Color(1,0,0));
    ball->setMaterialAmbient(0.5);
    ball->setTransform(Matrix::translation(0,-3.5,-0.5));
    w.addShape(ball);
    Ray r = Ray(Point(0,0,-3), Vector(0,-sqrt(2)/2, sqrt(2)/2));
    Intersection i = Intersection(sqrt(2), floor);
    Iset xs = { i };
    Icomps comps = i.prepComps(r, xs);
    Color c = w.shadeHit(comps, xs, 5);

    EXPECT_EQ(c, Color(0.93642, 0.68642, 0.68642) );
}

TEST(ReflectionTest, schlickTotalInternalReflection) {
    auto s = make_glass_sphere();
    Ray r = Ray(Point(0,0,sqrt(2)/2), Vector(0,1,0));
    Intersection i1(-sqrt(2)/2, s);
    Intersection i2(sqrt(2)/2, s);
    Iset xs = { i1, i2 };
    Icomps i2comps = i2.prepComps(r, xs);
    double reflectance = i2comps.schlick();
    EXPECT_FLOAT_EQ(reflectance, 1.0);
}

TEST(ReflectionTest, schlickPerpendicular) {
    auto s = make_glass_sphere();
    Ray r = Ray(Point(0,0,0), Vector(0,1,0));
    Intersection i1(-1, s);
    Intersection i2(1, s);
    Iset xs = { i1, i2 };
    Icomps comps = i2.prepComps(r, xs);
    EXPECT_FLOAT_EQ(comps.schlick(), 0.04);
}

TEST(ReflectionTest, schlickSmallAngle) {
    auto s = make_glass_sphere();
    Ray r = Ray(Point(0,0.99,-2), Vector(0,0,1));
    Intersection i1(1.8589, s);
    Iset xs = { i1 };
    Icomps comps = i1.prepComps(r, xs);
    EXPECT_FLOAT_EQ(comps.schlick(), 0.48873082);
}

TEST(ReflectionTest, shadeHitWithReflectiveTransparentMaterial) {
    World w;
    w.make_default();
    Ray r = Ray(Point(0,0,-3), Vector(0,-sqrt(2)/2, sqrt(2)/2));
    auto floor = Plane::make();
    floor->setTransform(Matrix::translation(0,-1,0));
    floor->setMaterialReflective(0.5);
    floor->setMaterialTransparency(0.5);
    floor->setMaterialRefractiveIndex(1.5);
    w.addShape(floor);
    auto ball = Sphere::make();
    ball->setMaterialColor(Color::Red);
    ball->setMaterialAmbient(0.5);
    ball->setTransform(Matrix::translation(0,-3.5,-0.5));
    w.addShape(ball);
    Intersection i1(sqrt(2), floor);
    Iset xs = { i1 };
    Icomps comps = i1.prepComps(r, xs);
    Color c = w.shadeHit(comps, xs, 5);
    EXPECT_EQ(c, Color(0.93391, 0.69643, 0.69243)); 
}