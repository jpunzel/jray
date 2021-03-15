#include "gtest/gtest.h"
#include "Tuple.h"
#include "Vector.h"
#include "Point.h"
#include "Ray.h"
#include "Sphere.h"
#include "World.h"
#include <iostream>
#include <memory>

TEST(ShadingTest, sphereNormals) {
    auto s = Sphere::make();
    Vector n = s->normalAt(Point(1,0,0));
    EXPECT_EQ(n, Vector(1,0,0));
    Vector n2 = s->normalAt(Point(0,1,0));
    EXPECT_EQ(n2, Vector(0,1,0));
    Vector n3 = s->normalAt(Point(0,0,1));
    EXPECT_EQ(n3, Vector(0,0,1));
    Vector n4 = s->normalAt(Point(sqrt(3)/3, sqrt(3)/3, sqrt(3)/3) );
    EXPECT_EQ(n4, Vector(sqrt(3)/3, sqrt(3)/3, sqrt(3)/3));
    EXPECT_EQ(n4, n4.normalize());
}

TEST(ShadingTest, translatedSphereNormals) {
    auto s = Sphere::make();
    s->setTransform(Matrix::translation(0,1,0));
    Vector n = s->normalAt(Point(0, 1.70711, -0.70711));
    EXPECT_EQ(n, Vector(0,0.70711,-0.70711));
}

TEST(ShadingTest, transformedSphereNormals) {
    auto s = Sphere::make();
    s->setTransform(Matrix::scaling(1,0.5,1) * Matrix::rotation_z(PI / 5));
    Vector n = s->normalAt(Point(0, sqrt(2)/2, -sqrt(2)/2));
    EXPECT_EQ(n, Vector(0,0.97014,-0.24254));
}

TEST(ShadingTest, reflectVector) {
    Vector v(1,-1,0);
    Vector n(0,1,0);
    EXPECT_EQ(v.reflect(n), Vector(1,1,0));
    Vector v2(0,-1,0);
    Vector n2(sqrt(2)/2, sqrt(2)/2,0);
    EXPECT_EQ(v2.reflect(n2), Vector(1,0,0));
}

TEST(ShadingTest, sphereMaterial) {
    Material m = Material();
    EXPECT_EQ(m.getColor(), Color(1,1,1));
    EXPECT_FLOAT_EQ(m.getAmbient(), 0.1);
    EXPECT_FLOAT_EQ(m.getDiffuse(), 0.9);
    EXPECT_FLOAT_EQ(m.getSpecular(), 0.9);
    EXPECT_FLOAT_EQ(m.getShininess(), 200);
    auto s = Sphere::make();
    Material m2 = s->getMaterial();
    EXPECT_EQ(m2, Material());
    m2.setAmbient(1.0);
    s->setMaterial(m2);
    EXPECT_EQ(s->getMaterial(), m2);
}

TEST(ShadingTest, phongReflection) {

    // Placeholder shape
    auto obj = Sphere::make();

    Material m = Material();
    Point pos = Point(0,0,0);
    // Eye is directly between light and surface:
    // We want reflection vector to directly hit the eye, thus
    // all material lighting values are at full strength.
    Vector eyev = Vector(0,0,-1);
    Vector normalv = Vector(0,0,-1);
    Light light = Light(Point(0,0,-10), Color(1,1,1));
    //EXPECT_EQ( m.lighting(obj, light, pos, eyev, normalv, false), Color(1.9,1.9,1.9));
    EXPECT_EQ( m.lighting(obj, light, pos, eyev, normalv, 1.0), Color(1.9,1.9,1.9));

    // Eye is 45 degrees off normal. Specular value goes to zero
    eyev = Vector(0, sqrt(2)/2, -sqrt(2)/2);
    //EXPECT_EQ( m.lighting(obj, light, pos, eyev, normalv, false), Color(1,1,1));
    EXPECT_EQ( m.lighting(obj, light, pos, eyev, normalv, 1.0), Color(1,1,1));

    // Move eye back to directly in front of surface,
    // but move light source up 10 units such that it is 45 degrees
    // off the normal vector
    eyev = Vector(0,0,-1);
    light = Light(Point(0,10,-10), Color(1,1,1));
    //EXPECT_EQ( m.lighting(obj, light, pos, eyev, normalv, false), Color(0.7364, 0.7364, 0.7364));
    EXPECT_EQ( m.lighting(obj, light, pos, eyev, normalv, 1.0), Color(0.7364, 0.7364, 0.7364));

    // Move eye directly in line with the reflection vector.
    // Specular component at full strength
    eyev = Vector(0, -sqrt(2)/2, -sqrt(2)/2);
    //EXPECT_EQ( m.lighting(obj, light, pos, eyev, normalv, false), Color(1.6364, 1.6364, 1.6364));
    EXPECT_EQ( m.lighting(obj, light, pos, eyev, normalv, 1.0), Color(1.6364, 1.6364, 1.6364));

    // Put light behind the surface entirely. Ambient is only remaining component
    eyev = Vector(0,0,-1);
    light = Light(Point(0,0,10), Color(1,1,1));
    //EXPECT_EQ( m.lighting(obj, light, pos, eyev, normalv, false), Color(0.1,0.1,0.1));
    EXPECT_EQ( m.lighting(obj, light, pos, eyev, normalv, 1.0), Color(0.1,0.1,0.1));
}

TEST(ShadingTest, lightingInShadow) {
    // Placeholder shape
    auto obj = Sphere::make();

    Material m = Material();
    Point pos = Point(0,0,0);
    Vector eyev = Vector(0,0,-1);
    Vector normalv = Vector(0,0,-1);
    Light light = Light(Point(0,0,-10), Color(1,1,1));
    //bool in_shadow = true;
    double in_shadow = 0.0;
    Color result = m.lighting(obj, light, pos, eyev, normalv, in_shadow);
    EXPECT_EQ(result, Color(0.1,0.1,0.1));
}

TEST(ShadingTest, lightingUsesLightIntensityToAttenuateColor) {
    World w;
    w.make_default();
    w.getLights()[0] = Light(Point(0,0,-10), Color(1,1,1));
    auto s = w.getShapes()[0];
    s->setMaterialAmbient(0.1);
    s->setMaterialDiffuse(0.9);
    s->setMaterialSpecular(0);
    s->setMaterialColor(Color(1,1,1));
    auto m = s->getMaterial();
    Point pt = Point(0,0,-1);
    Vector eyev = Vector(0,0,-1);
    Vector normalv = Vector(0,0,-1);

    Color result10 = m.lighting(s, w.getLights()[0], pt, eyev, normalv, 1.0);
    Color result05 = m.lighting(s, w.getLights()[0], pt, eyev, normalv, 0.5);
    Color result00 = m.lighting(s, w.getLights()[0], pt, eyev, normalv, 0.0);
    EXPECT_EQ(result10, Color(1,1,1));
    EXPECT_EQ(result05, Color(0.55,0.55,0.55));
    EXPECT_EQ(result00, Color(0.1,0.1,0.1));
}
