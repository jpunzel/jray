#pragma once

#include "Point.h"
#include "Vector.h"
#include "Matrix.h"
#include "Ray.h"
#define DEFAULT_WIDTH 800
#define DEFAULT_HEIGHT 600
#define DEFAULT_FOV 1.05

class Camera {
public:

    Camera(size_t width, size_t height, double fov) : 
                                                        hsize(width),
                                                        vsize(height),
                                                        transform(Matrix::identity(4)),
                                                        inverse_transform(Matrix::identity(4)),
                                                        fov(fov),
                                                        aperture_radius(0),
                                                        focal_length(1),
                                                        focal_samples(4),
                                                        supersampling(1)
    {
        setPixelSizeForFov(fov);
    }

    Camera(size_t width, size_t height, double fov, const Point &from, const Point &to, const Vector &up)
                                                    :   Camera(width, height, fov)
    {
        setTransform(from, to, up);
    }

    Camera() : Camera(DEFAULT_WIDTH, DEFAULT_HEIGHT, DEFAULT_FOV) { }

    static Matrix view_transform(const Point &from, const Point &to, const Vector &up);

    void setTransform(const Point &from, const Point &to, const Vector &up);
    void setTransform(const Matrix &m);
    void setFov(double f) { fov = f; setPixelSizeForFov(f); }
    void setFocalLength(double fl) { focal_length = fl; setPixelSizeForFov(fov); }
    void setAperture(double radius) { aperture_radius = radius; }
    void setFocalSamples(size_t samples) { focal_samples = samples; }
    void setSupersamplingLevel(size_t samples) { supersampling = samples; }
    double getFov() { return fov; }
    double getPixelSize() { return pixel_size; }
    // only use focal samples if we're using focal blur.
    size_t getFocalSamples() {
        if ( aperture_radius > 0 )
            return focal_samples;
        else
            return 1; 
    }
    size_t getSupersamplingLevel() { return supersampling; }
    void setPixelSize(double ps) { pixel_size = ps; }
    void setPixelSizeForFov(double fov) {
        // Compute pixel_size
        double half_view = tan(fov / 2) * focal_length;
        double aspect = (double)hsize / (double)vsize;
        if ( aspect >= 1 ) {
            half_width = half_view;
            half_height = half_view / aspect;
        } else {
            half_width = half_view * aspect;
            half_height = half_view;
        }
        pixel_size = ((half_width * 2) / hsize);
    }

    Ray ray_for_pixel(const size_t px, const size_t py, double px_offset = 0.5, double py_offset = 0.5) const;

    size_t hsize;
    size_t vsize;
    // The transform matrix represents how the world is transformed in front
    // of the camera. We apply its inverse when we generate the actual rays.
    Matrix transform;
    // Cache inverse transform: it's an expensive operation
    Matrix inverse_transform;

private:
    double fov;
    double half_width;
    double half_height;
    double pixel_size;

    double aperture_radius;
    double focal_length;
    size_t focal_samples;
    size_t supersampling;
};