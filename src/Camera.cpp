#include <random>
#include "Camera.h"
#include "math.h"


Matrix Camera::view_transform(const Point &from, const Point &to, const Vector &up)
{
    Vector forward = normalize(to - from);
    Vector upn = normalize(up);
    Vector left = cross(forward, upn);
    Vector true_up = cross(left, forward);

    Matrix orientation(4,4, { left.x(),      left.y(),     left.z(),    0,
                              true_up.x(),   true_up.y(),  true_up.z(), 0,
                              -forward.x(), -forward.y(), -forward.z(), 0,
                              0,             0,            0,           1 });
    
    return orientation * Matrix::translation(-from.x(), -from.y(), -from.z());
}

void Camera::setTransform(const Point &from, const Point &to, const Vector &up)
{
    Matrix T = Camera::view_transform(from, to, up);
    transform = T;
    inverse_transform = T.inverse();
}
void Camera::setTransform(const Matrix &m)
{
    transform = m;
    inverse_transform = m.inverse();
}

Ray Camera::ray_for_pixel(const size_t px, const size_t py, double px_offset, double py_offset) const
{
    // offset from edge of canvas to specified pixel offset
    // (default to 0.5 for pixel center when supersampling is not used)
    double xoffset = (px + px_offset) * pixel_size;
    double yoffset = (py + py_offset) * pixel_size;

    double world_x = half_width - xoffset;
    double world_y = half_height - yoffset;

    // Transform the canvas point and the aperture/origin (our "eye"),
    // then compute the ray's direction vector.
    // (canvas is focal_length away at z=-focal_length)
    Point pixel = inverse_transform * Point(world_x, world_y, -focal_length);

    Point origin;
    if (aperture_radius > 0) { // implement focal blur
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<> dis(-aperture_radius, aperture_radius);
        double aperture_x_offset = dis(gen);
        double aperture_y_offset = dis(gen);
        origin = inverse_transform * Point(aperture_x_offset, aperture_y_offset, 0);
    }
    else {
        origin = inverse_transform * Point(0,0,0);
    }
    
    Vector direction = normalize(pixel - origin);

    return Ray(origin, direction);
}