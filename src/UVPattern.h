#pragma once

#include "Color.h"
#include "Matrix.h"
#include "Canvas.h"
#include "util.h"
#include <memory>

#define CUBE_FACE_FRONT 0
#define CUBE_FACE_LEFT 1
#define CUBE_FACE_RIGHT 2
#define CUBE_FACE_UP 3
#define CUBE_FACE_DOWN 4
#define CUBE_FACE_BACK 5

// forward declaration of Shape
class Shape;

struct UVPoint {
    UVPoint(double u, double v) : u(u), v(v) { }
    double u;
    double v;
};

class UVPattern {
public:
    virtual Color uv_patternAt(const UVPoint &point) const = 0;
    static UVPoint sphericalMap(const Point &p);
    static UVPoint planarMap(const Point &p);
    static UVPoint cylindricalMap(const Point &p);

    static unsigned short cubeFaceFromPoint(const Point &p);
    static UVPoint cubeMapFront(const Point &p);
    static UVPoint cubeMapLeft(const Point &p);
    static UVPoint cubeMapRight(const Point &p);
    static UVPoint cubeMapUp(const Point &p);
    static UVPoint cubeMapDown(const Point &p);
    static UVPoint cubeMapBack(const Point &p);

protected:
    UVPattern() = default;
};

class UVCheckersPattern : public UVPattern {
public:
    static std::shared_ptr<UVPattern> make(unsigned width, unsigned height, Color a, Color b)
    {
        std::shared_ptr<UVPattern> ret(new UVCheckersPattern(width,height,a,b));
        return ret;
    }
    Color uv_patternAt(const UVPoint &point) const override;
private:
    UVCheckersPattern(unsigned width, unsigned height, Color a, Color b) : UVPattern(),
                                                                width(width),
                                                                height(height),
                                                                color_a(a),
                                                                color_b(b) { }
    unsigned width;
    unsigned height;
    Color color_a;
    Color color_b;
};

class UVAlignCheckPattern : public UVPattern {
public:
    static std::shared_ptr<UVPattern> make(Color main, Color ul, Color ur, Color bl, Color br)
    {
        std::shared_ptr<UVPattern> ret(new UVAlignCheckPattern(main,ul,ur,bl,br));
        return ret;
    }
    Color uv_patternAt(const UVPoint &point) const override;
private:
    UVAlignCheckPattern(Color main, Color ul, Color ur, Color bl, Color br) : UVPattern(),
                                                                              color_main(main),
                                                                              color_ul(ul),
                                                                              color_ur(ur),
                                                                              color_bl(bl),
                                                                              color_br(br) { }
    Color color_main;
    Color color_ul;
    Color color_ur;
    Color color_bl;
    Color color_br;
};

class UVImagePattern : public UVPattern {
public:
    static std::shared_ptr<UVPattern> make(const std::string &filename)
    {
        std::shared_ptr<UVPattern> ret(new UVImagePattern(filename));
        return ret;
    }
    Color uv_patternAt(const UVPoint &point) const override;

private:
    UVImagePattern(const std::string &filename) : UVPattern(), _canvas(filename) { }
    Canvas _canvas;
};