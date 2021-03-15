#include "World.h"
#include "Sphere.h"

void World::make_default() {
    auto s1 = Sphere::make();
    Material m1(Color(0.8, 1.0, 0.6), 0.1, 0.7, 0.2, 200, 0, 0, 1);
    s1->setMaterial(m1);
    
    auto s2 = Sphere::make();
    s2->setTransform(Matrix::scaling(0.5,0.5,0.5));

    shapes.push_back(s1);
    shapes.push_back(s2);

    lights.push_back(Light(Point(-10,10,-10), Color(1,1,1)));
}

void World::intersect(Ray ray, Iset &iset_out) const {
    //Iset local_intersects;
    for (auto s: shapes) {
        s->intersect(ray, iset_out);
    }
}

Color World::shadeHit(const Icomps &comps, Iset &iset, int remaining) const {
    Material m = comps.obj->getMaterial();
    Color surface, reflected, refracted;
    iset.clear();
    for (auto l : lights) {
        surface += m.lighting(comps.obj, l, comps.over_point, comps.eyev, comps.normalv, lightIntensityAt(comps.over_point, l) );
        //surface += m.lighting(comps.obj, l, comps.over_point, comps.eyev, comps.normalv, isShadowed(comps.over_point, iset, l.pos));
        iset.clear();
    }
    reflected = reflectedColor(comps, iset, remaining);
    iset.clear();
    refracted = refractedColor(comps, iset, remaining);

    if (comps.obj->getMaterial().getReflective() > 0 && comps.obj->getMaterial().getTransparency() > 0) {
        double reflectance = comps.schlick();
        return surface + reflected * reflectance + refracted * (1 - reflectance);
    } else {
        return surface + reflected + refracted;
    }
}

Color World::reflectedColor(const Icomps &comps, Iset &iset_out, int remaining) const {
    double refl = comps.obj->getMaterial().getReflective();
    if ( remaining <= 0 || doubleEqual(refl, 0) ) {
        return Color::Black;
    }
    Ray reflect_ray = Ray(comps.over_point, comps.reflectv);
    return colorAt(reflect_ray, iset_out, remaining - 1) * refl;
}

Color World::refractedColor(const Icomps &comps, Iset &iset_out, int remaining) const {
    double tr = comps.obj->getMaterial().getTransparency();
    if ( remaining <= 0 || tr == 0 ) {
        return Color::Black;
    }

    // Apply Snell's Law to determine if this is a case of total internal reflection
    // If so, return black. Total internal reflection means no refracted light will pass through.
    // Snell's Law: sin(theta[i]) / sin(theta[t]) == rindex_to / rindex_from
    //    theta[i] = angle of incidence
    //    theta[t] = angle of refraction <- want to determine this using formula:
    double n_ratio = comps.rindex_from / comps.rindex_to;
    double cos_i = dot(comps.eyev, comps.normalv); // cosine of angle is the same as dot product
    double sin2t = n_ratio*n_ratio * (1 - cos_i*cos_i); // trig identity to find sin^2(t)
    if (sin2t > 1) {
        return Color::Black;
    }

    double cos_t = sqrt(1.0 - sin2t);

    // Compute the direction of refracted ray
    Vector dir_refracted = comps.normalv * (n_ratio * cos_i - cos_t) - comps.eyev * n_ratio;
    // Create the refracted ray
    Ray refracted = Ray(comps.under_point , dir_refracted);

    // Now, as usual, find the color of this new refracted ray
    // (modified by our transparency)
    return colorAt(refracted, iset_out, remaining - 1) * tr;
}

Color World::colorAt(const Ray &r, Iset &iset_out, int remaining) const {
    intersect(r, iset_out);
    Intersection i = hit(iset_out);
    if ( i.isEmpty() ) {
        return Color::Black; // return black if no such intersection
    }
    return shadeHit(i.prepComps(r, iset_out), iset_out, remaining);
}

bool World::isShadowed(const Point &p, Iset &iset_out, const Point &lightpos) const {
    Vector v = lightpos - p;
    double distance = v.length();
    Vector direction = v.normalize();
    Ray r = Ray(p, direction);
    intersect(r, iset_out);
    Intersection ihit = hit(iset_out);
    if (!ihit.isEmpty() && ihit.t < distance && ihit.obj->castsShadow()) {
        return true;
    }
    return false;
}

double World::lightIntensityAt(const Point &p, const Light &l ) const
{
    Iset iset;
    double total = 0.0;

    for (int v = 0; v < l.vsteps; v++) {
        for (int u = 0; u < l.usteps; u++) {
            Point lightpos = l.pointAt(u,v);
            if ( !isShadowed(p, iset, lightpos) ) {
                total += 1.0;
            }
            iset.clear();
        }
    }
    return total / l.samples;
}