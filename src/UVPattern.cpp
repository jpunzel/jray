#include "UVPattern.h"
#include <math.h>

UVPoint UVPattern::sphericalMap(const Point &p)
{
    double theta = atan2(p.x(), p.z());
    Vector vec = Vector(p.x(), p.y(), p.z());
    double radius = vec.length();

    double phi = acos(p.y() / radius);

    double raw_u = theta / (2 * PI);

    double u = 1 - (raw_u + 0.5);
    double v = 1 - phi / PI;

    return UVPoint(u,v);
}

UVPoint UVPattern::planarMap(const Point &p)
{
    return UVPoint(
                    floatmod(p.x(), 1),
                    floatmod(p.z(), 1)
                  );
}

UVPoint UVPattern::cylindricalMap(const Point &p)
{
    double theta = atan2(p.x(), p.z());
    double raw_u = theta / (2 * PI);
    double u = 1 - (raw_u + 0.5);
    double v = fmod(p.y(),1);
    return UVPoint(u,v);
}

unsigned short UVPattern::cubeFaceFromPoint(const Point &p)
{
    double coord = std::max({ 
                             abs(p.x()),
                             abs(p.y()),
                             abs(p.z()),
                             });
    if ( coord == p.x() ) { return CUBE_FACE_RIGHT; }
    if ( coord == -p.x() ) { return CUBE_FACE_LEFT; }
    if ( coord == p.y() ) { return CUBE_FACE_UP; }
    if ( coord == -p.y() ) { return CUBE_FACE_DOWN; }
    if ( coord == p.z() ) { return CUBE_FACE_FRONT; }
    return CUBE_FACE_BACK;
}

UVPoint UVPattern::cubeMapFront(const Point &p)
{
    double u = floatmod( (p.x() + 1) , 2.0 ) / 2.0;
    double v = floatmod( (p.y() + 1) , 2.0 ) / 2.0;
    return UVPoint(u,v);
}
UVPoint UVPattern::cubeMapBack(const Point &p)
{
    double u = floatmod( (1 - p.x()) , 2.0 ) / 2.0;
    double v = floatmod( (p.y() + 1) , 2.0 ) / 2.0;
    return UVPoint(u,v);
}
UVPoint UVPattern::cubeMapRight(const Point &p)
{
    double u = floatmod( (1 - p.z()) , 2.0) / 2.0;
    double v = floatmod( (p.y() + 1) , 2.0) / 2.0;
    return UVPoint(u,v);
}
UVPoint UVPattern::cubeMapLeft(const Point &p)
{
    double u = floatmod( (p.z() - 1) , 2.0 ) / 2.0;
    double v = floatmod( (p.y() + 1) , 2.0 ) / 2.0;
    return UVPoint(u,v);
}
UVPoint UVPattern::cubeMapUp(const Point &p)
{
    double u = floatmod( (p.x() + 1) , 2.0 ) / 2.0;
    double v = floatmod( (1 - p.z()) , 2.0 ) / 2.0;
    return UVPoint(u,v);
}
UVPoint UVPattern::cubeMapDown(const Point &p)
{
    double u = floatmod( (p.x() - 1) , 2.0 ) / 2.0;
    double v = floatmod( (p.z() + 1) , 2.0 ) / 2.0;
    return UVPoint(u,v);
}

Color UVCheckersPattern::uv_patternAt(const UVPoint &point) const
{
    int u2 = floor(point.u * width);
    int v2 = floor(point.v * height);

    if ( (u2+v2) % 2  == 0 ) {
        return color_a;
    }
    return color_b;
}

Color UVAlignCheckPattern::uv_patternAt(const UVPoint &point) const
{
    if (point.v > 0.8) {
        if (point.u < 0.2) { return color_ul; }
        if (point.u > 0.8) { return color_ur; }
    } else if (point.v < 0.2) {
        if (point.u < 0.2) { return color_bl; }
        if (point.u > 0.8) { return color_br; }
    }
    return color_main;
}

Color UVImagePattern::uv_patternAt(const UVPoint &point) const
{
    // flip image coordinates
    double u = point.u;
    double v = 1 - point.v;

    double x = u * (_canvas.get_width() - 1);
    double y = v * (_canvas.get_height() - 1);

    return _canvas.get_pixel(round(x), round(y));
}