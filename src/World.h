#pragma once

#include "Shape.h"
#include "Ray.h"
#include <vector>
#define REFLECTION_RECURSION_LIMIT 4

class World {
public:

    // default constructor and copy-control are fine here
    void make_default(); // adds a light and some "default" shapes for testing
    void intersect(Ray ray, Iset &iset_out) const;

    // These get* functions are for testing, but not used generally.
    // Beware of object lifetime issues with the returned references
    std::vector<std::shared_ptr<Shape>>& getShapes() { return shapes; }
    std::vector<Light>& getLights() { return lights; }

    void addLight(const Light &l) { lights.push_back(l); }
    void addShape(std::shared_ptr<Shape> shape) { shapes.push_back(shape); }
    Color shadeHit(const Icomps &comps, Iset &iset, int remaining) const;
    Color reflectedColor(const Icomps &comps, Iset &iset_out, int remaining) const;
    Color refractedColor(const Icomps &comps, Iset &iset_out, int remaining) const;
    Color colorAt(const Ray &r, Iset &iset_out, int remaining = REFLECTION_RECURSION_LIMIT) const;
    bool isShadowed(const Point &p, Iset &iset_out, const Point &lightpos) const;
    double lightIntensityAt(const Point &p, const Light &l) const;

private:
    std::vector<std::shared_ptr<Shape>> shapes;
    std::vector<Light> lights;
};
