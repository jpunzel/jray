#pragma once
#include "Point.h"
#include "Vector.h"
#include "Color.h"
#include <random>

class Light {
public:
    Light(const Point &p, const Color &i) : pos(p),
                                            uvec(Vector(0,0,0)),
                                            vvec(Vector(0,0,0)),
                                            usteps(1),
                                            vsteps(1),
                                            intensity(i),
                                            samples(1),
                                            jitter(false)
    { 
        samplePoints.push_back(pointAt(0,0));
    }
    Light(const Point &p, const Vector &uvec, int usteps, const Vector &vvec, int vsteps, const Color &i, bool jitter) :
                                            pos(p),
                                            uvec(uvec),
                                            usteps(usteps),
                                            vvec(vvec),
                                            vsteps(vsteps),
                                            intensity(i),
                                            samples(usteps*vsteps),
                                            jitter(jitter)
    {
        for (int v = 0; v < vsteps; v++) {
            for (int u = 0; u < usteps; u++) {
                samplePoints.push_back(pointAt(u,v));
            }
        }
    }

    Point pointAt(double u, double v) const
    {
        if (jitter) {
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_real_distribution<double> dis(0.0, 1.0);
            return pos + (u + dis(gen))*(uvec/usteps) + (v + dis(gen))*(vvec/vsteps);
        } else {
            return pos + (u + 0.5)*(uvec/usteps) + (v + 0.5)*(vvec/vsteps);
        }
    }

    bool jitter;
    Point pos;
    Vector uvec;
    int usteps;
    Vector vvec;
    int vsteps;
    Color intensity;
    int samples;
    std::vector<Point> samplePoints;
};