#include "Material.h"
#include "Vector.h"
#include "Shape.h"
#include "util.h"
#include <math.h>

//Color Material::lighting(const std::shared_ptr<Shape> &obj, const Light &light, const Point &p, const Vector &eye, const Vector &n, double intensity) const
//{
//    Color color;
//    if ( m_pattern_ptr ) {
//        color = m_pattern_ptr->patternAtShape(obj, p);
//    } else {
//        color = m_color;
//    }
//    Color effective_color = color * light.intensity;
//    Vector lightv = normalize(light.pos - p);
//    
//    Color ambient = effective_color * m_ambient;
////  if (in_shadow) {
////      return ambient;
////  }
//
//    double light_dot_normal; 
//    Color diffuse;
//    Color specular;
//    Vector reflectv;
//    double reflect_dot_eye;
//    double factor;
//
//    if ((light_dot_normal = dot(lightv, n)) < 0) {
//        diffuse = Color(0,0,0);
//        specular = Color(0,0,0);
//    } else {
//        diffuse = effective_color * m_diffuse * light_dot_normal ;
//        reflectv = reflect(-lightv, n);
//        if ((reflect_dot_eye = dot(reflectv, eye)) <= 0) {
//            specular = Color(0,0,0);
//        } else {
//            factor = pow(reflect_dot_eye, m_shininess);
//            specular = light.intensity * (m_specular * factor);
//        }
//    }
//    return ambient + diffuse*intensity + specular*intensity;
//}

Color Material::lighting(const std::shared_ptr<Shape> &obj, const Light &light, const Point &p, const Vector &eye, const Vector &n, double intensity) const
{
    Color color;
    if ( m_pattern_ptr ) {
        color = m_pattern_ptr->patternAtShape(obj, p);
    } else {
        color = m_color;
    }
    Color effective_color = color * light.intensity;
    Color ambient = effective_color * m_ambient;

    Color sum = Color(0,0,0);

    for (auto sample : light.samplePoints) {
        Vector lightv = normalize(sample - p);

        double light_dot_normal = dot(lightv, n);
        Color diffuse;
        Color specular;
        Vector reflectv;
        double reflect_dot_eye;
        double factor;

        if (light_dot_normal < 0 || doubleEqual(intensity, 0) ) {
            diffuse = Color(0,0,0);
            specular = Color(0,0,0);
        } else {
            diffuse = effective_color * m_diffuse * light_dot_normal ;
            reflectv = reflect(-lightv, n);
            if ((reflect_dot_eye = dot(reflectv, eye)) <= 0) {
                specular = Color(0,0,0);
            } else {
                factor = pow(reflect_dot_eye, m_shininess);
                specular = light.intensity * (m_specular * factor);
            }
        }
        sum += diffuse;
        sum += specular;
    }
    return ambient + (sum / light.samples) * intensity;
}