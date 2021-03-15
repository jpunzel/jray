#pragma once
#include "Color.h"
#include "Pattern.h"
#include "Light.h"
#include "Vector.h"
#include "Point.h"
#include <memory>


class Material {
public:
    Material(): m_color(Color(1,1,1)),
                m_ambient(0.1),
                m_diffuse(0.9),
                m_specular(0.9),
                m_shininess(200),
                m_reflective(0.0),
                m_transparency(0.0),
                m_refractive_index(1.0) { }
    Material(const Color &c): m_color(c),
                       m_ambient(0.1),
                       m_diffuse(0.9),
                       m_specular(0.9),
                       m_shininess(200),
                       m_reflective(0.0),
                       m_transparency(0.0),
                       m_refractive_index(1.0) { } 
    Material(const std::shared_ptr<Pattern> &ptr): m_pattern_ptr(ptr),
                       m_ambient(0.1),
                       m_diffuse(0.9),
                       m_specular(0.9),
                       m_shininess(200),
                       m_reflective(0.0),
                       m_transparency(0.0),
                       m_refractive_index(1.0) { } 
    Material(const Color &c, double a, double d, double spec, double sh, double refl, double tr, double rindex) :
                m_color(c), 
                m_ambient(a), 
                m_diffuse(d),
                m_specular(spec), 
                m_shininess(sh), 
                m_reflective(refl),
                m_transparency(tr),
                m_refractive_index(rindex) { }
    Material(const std::shared_ptr<Pattern> &ptr, double a, double d, double spec, double sh, double refl, double tr, double rindex) :
                m_pattern_ptr(ptr),
                m_ambient(a),
                m_diffuse(d),
                m_specular(spec),
                m_shininess(sh),
                m_reflective(refl),
                m_transparency(tr),
                m_refractive_index(rindex) { }
    friend bool operator==(const Material &m1, const Material &m2) {
        return (m1.m_color == m2.m_color &&
                m1.m_ambient == m2.m_ambient &&
                m1.m_diffuse == m2.m_diffuse &&
                m1.m_specular == m2.m_specular &&
                m1.m_shininess == m2.m_shininess &&
                m1.m_reflective == m2.m_reflective &&
                m1.m_transparency == m2.m_transparency &&
                m1.m_refractive_index == m2.m_refractive_index );
    }
    friend bool operator!=(const Material &m1, const Material &m2) { return !(m1 == m2); }
    
    Color lighting(const std::shared_ptr<Shape> &obj, const Light &l, const Point &p, const Vector &eye, const Vector &n, double intensity) const;
    
    void setColor(const Color &c) { m_color = c; }
    void setPattern(const std::shared_ptr<Pattern> &p) { m_pattern_ptr = p; }
    void setAmbient(double ambient) { m_ambient = ambient; }
    void setDiffuse(double diffuse) { m_diffuse = diffuse; }
    void setSpecular(double specular) { m_specular = specular; }
    void setShininess(double shininess) { m_shininess = shininess; }
    void setReflective(double reflective) { m_reflective = reflective; }
    void setTransparency(double transparency) { m_transparency = transparency; }
    void setRefractiveIndex(double rindex) { m_refractive_index = rindex; }

    Color getColor() { return m_color; }
    std::shared_ptr<Pattern> getPattern() { return m_pattern_ptr; }
    double getAmbient() { return m_ambient; }
    double getDiffuse() { return m_diffuse; }
    double getSpecular() { return m_specular; }
    double getShininess() { return m_shininess; }
    double getReflective() { return m_reflective; }
    double getTransparency() { return m_transparency; }
    double getRefractiveIndex() { return m_refractive_index; }

private:
    std::shared_ptr<Pattern> m_pattern_ptr;
    Color m_color;
    double m_ambient;
    double m_diffuse;
    double m_specular;
    double m_shininess;
    double m_reflective;
    double m_transparency;
    double m_refractive_index;
};
