#include "gtest/gtest.h"
#include "World.h"
#include "Camera.h"
#include "Sphere.h"
#include "util.h"
#include <iostream>
#include <memory>

TEST(SceneTest, defaultWorld) {
    World w;
    w.make_default();
    Material m(Color(0.8, 1.0, 0.6), 0.1, 0.7, 0.2, 200, 0, 0, 1);

    EXPECT_EQ(w.getShapes()[0]->getMaterial(), m);
    EXPECT_EQ(w.getShapes()[1]->getTransform(), Matrix::scaling(0.5,0.5,0.5));
}

TEST(SceneTest, intersectWorld) {
    World w;
    w.make_default();

    Iset iset;
    w.intersect(Ray(Point(0,0,-5), Vector(0,0,1)), iset);
    EXPECT_EQ(iset.size(), 4);
    std::vector<double> expected{4, 4.5, 5.5, 6};
    size_t n = 0;
    for (auto i: iset) {
        EXPECT_EQ(expected[n], i.t);
        n++;
    }
}

TEST(SceneTest, precomputeIntersection) {
    Ray r(Point(0,0,-5) , Vector(0,0,1) );
    auto s = Sphere::make();
    Intersection i(4.0, s);

    Icomps comps = i.prepComps(r);
    EXPECT_EQ(comps.t, i.t);
    EXPECT_EQ(comps.obj, i.obj);
    EXPECT_EQ(comps.point, Point(0,0,-1));
    EXPECT_EQ(comps.eyev, Vector(0,0,-1));
    EXPECT_EQ(comps.normalv, Vector(0,0,-1));
    EXPECT_FALSE(comps.inside);

}

TEST(SceneTest, precomputeIntersectionInside) {
    Ray r(Point(0,0,0), Vector(0,0,1));
    auto s = Sphere::make();
    Intersection i(1,s);
    Icomps comps = i.prepComps(r);

    EXPECT_EQ(comps.point, Point(0,0,1));
    EXPECT_EQ(comps.eyev, Vector(0,0,-1));
    EXPECT_EQ(comps.normalv, Vector(0,0,-1));
    EXPECT_TRUE(comps.inside);
}

TEST(SceneTest, shadeWorldIntersection) {
    World w;
    w.make_default();

    Ray r(Point(0,0,-5), Vector(0,0,1));
    auto s = w.getShapes()[0];
    Intersection i(4,s);
    Icomps comps = i.prepComps(r);
    Iset iset;
    Color c = w.shadeHit(comps, iset, 10);
    EXPECT_EQ(c, Color(0.38066,0.47583,0.2855));
}

TEST(SceneTest, shadeWorldIntersectionInside) {
    World w;
    w.make_default();
    w.getLights().clear();
    w.addLight(Light(Point(0,0.25,0), Color(1,1,1)) );
    Ray r(Point(0,0,0), Vector(0,0,1));
    auto s = w.getShapes()[1];
    Intersection i(0.5,s);
    Icomps comps = i.prepComps(r);
    Iset iset;
    Color c = w.shadeHit(comps, iset, 10);
    EXPECT_EQ(c, Color(0.90498,0.90498,0.90498));
}

TEST(SceneTest, colorAtRayMissed) {
    World w;
    w.make_default();
    Ray r(Point(0,0,-5), Vector(0,1,0));
    Iset iset;
    Color c = w.colorAt(r, iset);
    EXPECT_EQ(c, Color(0,0,0));
}

TEST(SceneTest, colorAtRayHit) {
    World w;
    w.make_default();
    Ray r(Point(0,0,-5), Vector(0,0,1));
    Iset iset;
    Color c = w.colorAt(r, iset);
    EXPECT_EQ(c, Color(0.38066,0.47583,0.2855));
}

TEST(SceneTest, colorAtRayHitInside) {
    World w;
    w.make_default();
    auto outer = w.getShapes()[0];
    auto inner = w.getShapes()[1];
    Material outer_m = outer->getMaterial();
    Material inner_m = inner->getMaterial();
    outer_m.setAmbient(1.0);
    inner_m.setAmbient(1.0);
    outer->setMaterial(outer_m);
    inner->setMaterial(inner_m);
    Ray r(Point(0,0,0.75), Vector(0,0,-1));
    Iset iset;
    Color c = w.colorAt(r, iset);
    EXPECT_EQ(c, inner->getMaterial().getColor());
}

TEST(SceneTest, viewTransformDefault) {
    Point from(0,0,0);
    Point to(0,0,-1);
    Vector up(0,1,0);

    Matrix T = Camera::view_transform(from, to, up);
    EXPECT_EQ(T, Matrix::identity(4));
}

TEST(SceneTest, viewTransformPositiveZ) {
    Point from(0,0,0);
    Point to(0,0,1);
    Vector up(0,1,0);
    Matrix T = Camera::view_transform(from, to, up);
    EXPECT_EQ(T, Matrix::scaling(-1, 1, -1));
}

TEST(SceneTest, viewTransformWorld) {
    // Verify that view transform moves the world itself
    Point from(0,0,8);
    Point to(0,0,0);
    Vector up(0,1,0);

    Matrix T = Camera::view_transform(from, to, up);
    EXPECT_EQ(T, Matrix::translation(0,0,-8));
}

TEST(SceneTest, viewTransformArbitrary) {
    Point from(1,3,2);
    Point to(4,-2,8);
    Vector up(1,1,0);

    Matrix T = Camera::view_transform(from, to, up);
    Matrix expected = Matrix(4,4, { -0.50709, 0.50709, 0.67612, -2.36643,
                                    0.76772, 0.60609, 0.12122, -2.82843,
                                    -0.35857, 0.59761, -0.71714, 0,
                                    0, 0, 0, 1 });
    EXPECT_EQ(T, expected);

}

TEST(SceneTest, constructCamera) {
    Camera c = Camera(160,120,PI/2);
    EXPECT_EQ(c.hsize, 160);
    EXPECT_EQ(c.vsize, 120);
    EXPECT_FLOAT_EQ(c.getFov(), PI/2);
    EXPECT_EQ(c.transform, Matrix::identity(4));
}

TEST(SceneTest, cameraPixelSizeHorizontalAspect) {
    Camera c = Camera(200, 125, PI/2);
    EXPECT_FLOAT_EQ(c.getPixelSize(), 0.01);
}
TEST(SceneTest, cameraPixelSizeVerticalAspect) {
    Camera c = Camera(125, 200, PI/2);
    EXPECT_FLOAT_EQ(c.getPixelSize(), 0.01);
}

TEST(SceneTest, constructRayThroughCenter) {
    Camera c = Camera(201, 101, PI/2);
    Ray r = c.ray_for_pixel(100, 50);
    EXPECT_EQ(r.origin, Point(0,0,0));
    EXPECT_EQ(r.dir, Vector(0,0,-1));
}

TEST(SceneTest, constructRayThroughCorner) {
    Camera c = Camera(201, 101, PI/2);
    Ray r = c.ray_for_pixel(0, 0);
    EXPECT_EQ(r.origin, Point(0,0,0));
    EXPECT_EQ(r.dir, Vector(0.66519, 0.33259, -0.66851));
}

TEST(SceneTest, constructRayThroughTransformedCenter) {
    Camera c = Camera(201, 101, PI/2);
    c.setTransform(Matrix::rotation_y(PI/4) * Matrix::translation(0,-2,5));
    Ray r = c.ray_for_pixel(100,50);
    EXPECT_EQ(r.origin, Point(0,2,-5));
    EXPECT_EQ(r.dir, Vector(sqrt(2)/2, 0, -sqrt(2)/2));
}

//TEST(SceneTest, noShadowWhenNothingColinear) {
//    World w;
//    w.make_default();
//    Point p = Point(0,10,0);
//    Iset iset;
//    EXPECT_EQ(w.isShadowed(p, iset, w.getLights()[0]), false);
//}
//TEST(SceneTest, shadowWhenBetweenPointAndLight) {
//    World w;
//    w.make_default();
//    Point p = Point(10,-10,10);
//    Iset iset;
//    EXPECT_EQ(w.isShadowed(p, iset, w.getLights()[0]), true);
//}
//TEST(SceneTest, noShadowWhenObjectBehindLight) {
//    World w;
//    w.make_default();
//    Point p = Point(-20,20,-20);
//    Iset iset;
//    EXPECT_EQ(w.isShadowed(p, iset, w.getLights()[0]), false);
//}
//TEST(SceneTest, noShadowWhenObjectBehindPoint) {
//    World w;
//    w.make_default();
//    Point p = Point(-2,2,-2);
//    Iset iset;
//    EXPECT_EQ(w.isShadowed(p, iset, w.getLights()[0]), false);
//}


TEST(SceneTest, isShadowedTestsOcclusionBetweenTwoPoints) {
    World w;
    w.make_default();
    Point lightpos = Point(-10, -10, -10);
    Iset iset;
    std::vector<bool> expected_results = { false, true, false, false };
    int i = 0;
    for ( auto p : {
                    Point(-10, -10, 10),
                    Point(10, 10, 10),
                    Point(-20, -20, -20),
                    Point(-5, -5, -5) }) {
        EXPECT_EQ( w.isShadowed(p, iset, lightpos), expected_results[i++]);
        iset.clear();
    }


}


TEST(SceneTest, shadeHitInShadow) {
    World w;
    w.addLight(Light(Point(0,0,-10), Color(1,1,1)));
    auto s1 = Sphere::make();
    auto s2 = Sphere::make();
    s2->setTransform(Matrix::translation(0,0,10));
    w.addShape(s1);
    w.addShape(s2);
    Ray r = Ray(Point(0,0,5), Vector(0,0,1));
    Intersection i(4,s2);
    Icomps comps = i.prepComps(r);
    Iset iset;
    EXPECT_EQ(w.shadeHit(comps, iset, 10), Color(0.1,0.1,0.1));
}

TEST(SceneTest, offsetHitPointToPreventSelfShadows) {
    Ray r = Ray(Point(0,0,-5), Vector(0,0,1));
    auto s = Sphere::make();
    s->setTransform(Matrix::translation(0,0,1));
    Intersection i(5,s);
    Icomps comps = i.prepComps(r);
    EXPECT_LT(comps.over_point.z(), -EPSILON / 2);
    EXPECT_GT(comps.point.z(), comps.over_point.z());
}

TEST(SceneTest, pointOnAreaLight) {
    Point corner = Point(0,0,0);
    Vector v1 = Vector(2,0,0);
    Vector v2 = Vector(0,0,1);
    Light light = Light(corner, v1, 4, v2, 2, Color(1,1,1), false);
    Point p1 = light.pointAt(0,0);
    Point p2 = light.pointAt(1,0);
    Point p3 = light.pointAt(0,1);
    Point p4 = light.pointAt(2,0);
    Point p5 = light.pointAt(3,1);
    EXPECT_EQ(p1, Point(0.25, 0, 0.25) );
    EXPECT_EQ(p2, Point(0.75, 0, 0.25) );
    EXPECT_EQ(p3, Point(0.25, 0, 0.75) );
    EXPECT_EQ(p4, Point(1.25, 0, 0.25) );
    EXPECT_EQ(p5, Point(1.75, 0, 0.75) );
}

TEST(SceneTest, areaLightIntensity) {
    World w;
    w.make_default();
    Point corner = Point(-0.5, -0.5, -5);
    Vector v1 = Vector(1,0,0);
    Vector v2 = Vector(0,1,0);
    Light l = Light(corner, v1, 2, v2, 2, Color(1,1,1), false);
    Point p1 = Point(0,0,2);
    Point p2 = Point(1,-1,2);
    Point p3 = Point(1.5,0,2);
    Point p4 = Point(1.25,1.25,3);
    Point p5 = Point(0,0,-2);
    EXPECT_EQ( 0.0, w.lightIntensityAt(p1, l));
    EXPECT_EQ( 0.25, w.lightIntensityAt(p2, l));
    EXPECT_EQ( 0.5, w.lightIntensityAt(p3, l));
    EXPECT_EQ( 0.75, w.lightIntensityAt(p4, l));
    EXPECT_EQ( 1.0, w.lightIntensityAt(p5, l));
}