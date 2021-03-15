#pragma once

#include "Ray.h"
#include "Material.h"
#include "BoundingBox.h"
#include <utility>
#include <memory>

class Group;

class Shape : public std::enable_shared_from_this<Shape> {
public:
    void setTransform(const Matrix &M) {
        transform = M;
        inverse_transform = M.inverse();
    }
    void setMaterial(const Material &m) {
        material = m;
        material_modified = true;
    }
    const Matrix& getTransform() const { return transform; }
    const Matrix& getInverseTransform() const { return inverse_transform; }
    Material getMaterial() const;
    bool castsShadow() { return shadows_enabled; }
    bool castsShadow(bool enabled) {
        shadows_enabled = enabled;
        return shadows_enabled;
    }

    Vector normalAt(const Point &p, const Intersection *const ip = nullptr) const;
    bool intersect(const Ray &r, Iset &iset);

    virtual Vector localNormalAt(const Point &obj_p, const Intersection *const ip = nullptr) const = 0;
    virtual bool localIntersect(const Ray &ray_transformed, Iset &iset_out) = 0;

    // return bounding box within object space
    virtual BoundingBox bounds() const = 0;

    // divides shape (for Groups and CSG. Primitive shapes will do nothing)
    virtual void divide(size_t threshold) = 0;

    // for aggregate shapes (Groups/CSG), test if children contain shape
    // (Primitive shapes will do nothing)
    virtual bool includes(const std::shared_ptr<Shape> &shape) = 0;

    // return bounding box "outside of" object space
    BoundingBox parentBounds() const {
        return bounds().transform(transform);
    }

    // Some shortcut functions to directly modify our local Material
    void setMaterialColor(const Color &c) { material.setColor(c); material_modified = true; }
    void setMaterialPattern(const std::shared_ptr<Pattern> &p) { material.setPattern(p); material_modified = true; }
    void setMaterialAmbient(double ambient) { material.setAmbient(ambient); material_modified = true; }
    void setMaterialDiffuse(double diffuse) { material.setDiffuse(diffuse); material_modified = true; }
    void setMaterialSpecular(double specular) { material.setSpecular(specular); material_modified = true; }
    void setMaterialShininess(double shininess) { material.setShininess(shininess); material_modified = true; }
    void setMaterialReflective(double reflective) { material.setReflective(reflective); material_modified = true; }
    void setMaterialRefractiveIndex(double rindex) { material.setRefractiveIndex(rindex); material_modified = true; }
    void setMaterialTransparency(double transparency) { material.setTransparency(transparency); material_modified = true; }

    std::shared_ptr<Shape> getParent() const { return parent; }
    void setParent(const std::shared_ptr<Shape> &p) {
        parent = p;
    }
    Point world_to_object(Point p) const;
    Vector normal_to_world(Vector n) const;

    BoundingBox bbox; // Only needed by Group and CSG, but we'll keep it in the base class
                      // for simplicity's sake when accessing via generic Shape pointer

protected:
    // We only want to use/access Shapes via shared pointers to this base class.
    // Keep constructors protected here and private in derived classes.
    // Derived classes will define static make() functions which will
    // return shared_ptr<Shape>.
    Shape() : transform(Matrix::identity(4)),
              inverse_transform(Matrix::identity(4)),
              material(Material()),
              shadows_enabled(true),
              material_modified(false),
              parent(nullptr) { }
    Shape(const Matrix &M) : transform(M),
                             inverse_transform(M.inverse()),
                             material(Material()),
                             shadows_enabled(true),
                             material_modified(false),
                             parent(nullptr) { }

    Matrix transform;
    // Cache the inverse of transform matrix for performance
    Matrix inverse_transform;
    Material material;
    bool shadows_enabled;
    bool material_modified;
    std::shared_ptr<Shape> parent;
};