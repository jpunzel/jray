#pragma once

#include "Color.h"
#include "Matrix.h"
#include "UVPattern.h"
#include <memory>
#include <functional>

// forward declaration of Shape
class Shape;

class Pattern {
public:
    void setTransform(const Matrix &M) {
        transform = M;
        inverse_transform = M.inverse();
    }
    Matrix getTransform() const { return transform; }

    Color patternAtShape(const std::shared_ptr<Shape> &sp, const Point &p) const;
    virtual Color patternAt(const Point &pattern_point) const = 0;

protected:
    // We only want to use/access Patterns via shared pointers to this base class.
    // Keep constructors protected here and private in derived classes.
    // Derived classes will define static make() functions which will
    // return shared_ptr<Pattern>
    Pattern() : transform(Matrix::identity(4)),
                inverse_transform(Matrix::identity(4)) { }
    Pattern(const Matrix &T) : transform(T),
                               inverse_transform(T.inverse()) { }
    Matrix transform;
    Matrix inverse_transform;
};

class SolidPattern : public Pattern {
public:

    static std::shared_ptr<Pattern> make(const Color &c)
    {
        std::shared_ptr<Pattern> ret(new SolidPattern(c));
        return ret;
    }
    static std::shared_ptr<Pattern> make(const Matrix &T, const Color &c)
    {
        std::shared_ptr<Pattern> ret(new SolidPattern(T,c));
        return ret;
    }

    Color patternAt(const Point &pp) const override;

private:
    SolidPattern(const Color &c) : Pattern(), color(c) { }
    SolidPattern(const Matrix &T, const Color &c) : Pattern(T), color(c) { }
    Color color;
};

class BlendedPattern : public Pattern {
public:

    static std::shared_ptr<Pattern> make(const Color &a, const Color &b)
    {
        std::shared_ptr<Pattern> ret(new BlendedPattern(a,b));
        return ret;
    }
    static std::shared_ptr<Pattern> make(const Matrix &T, const Color &a, const Color &b)
    {
        std::shared_ptr<Pattern> ret(new BlendedPattern(T,a,b));
        return ret;
    }
    static std::shared_ptr<Pattern> make(const std::shared_ptr<Pattern> &a, const std::shared_ptr<Pattern> &b)
    {
        std::shared_ptr<Pattern> ret(new BlendedPattern(a,b));
        return ret;
    }
    static std::shared_ptr<Pattern> make(const Matrix &T,
                                         const std::shared_ptr<Pattern> &a,
                                         const std::shared_ptr<Pattern> &b)
    {
        std::shared_ptr<Pattern> ret(new BlendedPattern(T,a,b));
        return ret;
    }

    Color patternAt(const Point &pp) const override;

private:
    BlendedPattern(const Color &a, const Color &b) : Pattern(),
                                                    pattern_a(SolidPattern::make(a)),
                                                    pattern_b(SolidPattern::make(b)) { }
    BlendedPattern(const Matrix &T, const Color &a, const Color &b) : Pattern(T),
                                                                     pattern_a(SolidPattern::make(a)),
                                                                     pattern_b(SolidPattern::make(b)) { }
    BlendedPattern(const std::shared_ptr<Pattern> &a, const std::shared_ptr<Pattern> &b) :
            Pattern(), pattern_a(a), pattern_b(b) { }
    BlendedPattern(const Matrix &T, const std::shared_ptr<Pattern> &a, const std::shared_ptr<Pattern> &b) :
            Pattern(T), pattern_a(a), pattern_b(b) { }

    std::shared_ptr<Pattern> pattern_a;
    std::shared_ptr<Pattern> pattern_b;
};

class StripePattern : public Pattern {
public:

    static std::shared_ptr<Pattern> make(const Color &a, const Color &b)
    {
        std::shared_ptr<Pattern> ret(new StripePattern(a,b));
        return ret;
    }
    static std::shared_ptr<Pattern> make(const Matrix &T, const Color &a, const Color &b)
    {
        std::shared_ptr<Pattern> ret(new StripePattern(T,a,b));
        return ret;
    }
    static std::shared_ptr<Pattern> make(const std::shared_ptr<Pattern> &a, const std::shared_ptr<Pattern> &b)
    {
        std::shared_ptr<Pattern> ret(new StripePattern(a,b));
        return ret;
    }
    static std::shared_ptr<Pattern> make(const Matrix &T,
                                         const std::shared_ptr<Pattern> &a,
                                         const std::shared_ptr<Pattern> &b)
    {
        std::shared_ptr<Pattern> ret(new StripePattern(T,a,b));
        return ret;
    }

    Color patternAt(const Point &pp) const override;

private:
    StripePattern(const Color &a, const Color &b) : Pattern(),
                                                    pattern_a(SolidPattern::make(a)),
                                                    pattern_b(SolidPattern::make(b)) { }
    StripePattern(const Matrix &T, const Color &a, const Color &b) : Pattern(T),
                                                                     pattern_a(SolidPattern::make(a)),
                                                                     pattern_b(SolidPattern::make(b)) { }
    StripePattern(const std::shared_ptr<Pattern> &a, const std::shared_ptr<Pattern> &b) :
            Pattern(), pattern_a(a), pattern_b(b) { }
    StripePattern(const Matrix &T, const std::shared_ptr<Pattern> &a, const std::shared_ptr<Pattern> &b) :
            Pattern(T), pattern_a(a), pattern_b(b) { }

    std::shared_ptr<Pattern> pattern_a;
    std::shared_ptr<Pattern> pattern_b;
};

class GradientPattern : public Pattern {
public:

    static std::shared_ptr<Pattern> make(const Color &a, const Color &b)
    {
        std::shared_ptr<Pattern> ret(new GradientPattern(a,b));
        return ret;
    }
    static std::shared_ptr<Pattern> make(const Matrix &T, const Color &a, const Color &b)
    {
        std::shared_ptr<Pattern> ret(new GradientPattern(T,a,b));
        return ret;
    }
    static std::shared_ptr<Pattern> make(const std::shared_ptr<Pattern> &a, const std::shared_ptr<Pattern> &b)
    {
        std::shared_ptr<Pattern> ret(new GradientPattern(a,b));
        return ret;
    }
    static std::shared_ptr<Pattern> make(const Matrix &T,
                                         const std::shared_ptr<Pattern> &a,
                                         const std::shared_ptr<Pattern> &b)
    {
        std::shared_ptr<Pattern> ret(new GradientPattern(T,a,b));
        return ret;
    }

    Color patternAt(const Point &pp) const override;

private:
    GradientPattern(const Color &a, const Color &b) : Pattern(),
                                                    pattern_a(SolidPattern::make(a)),
                                                    pattern_b(SolidPattern::make(b)) { }
    GradientPattern(const Matrix &T, const Color &a, const Color &b) : Pattern(T),
                                                                     pattern_a(SolidPattern::make(a)),
                                                                     pattern_b(SolidPattern::make(b)) { }
    GradientPattern(const std::shared_ptr<Pattern> &a, const std::shared_ptr<Pattern> &b) :
            Pattern(), pattern_a(a), pattern_b(b) { }
    GradientPattern(const Matrix &T, const std::shared_ptr<Pattern> &a, const std::shared_ptr<Pattern> &b) :
            Pattern(T), pattern_a(a), pattern_b(b) { }

    std::shared_ptr<Pattern> pattern_a;
    std::shared_ptr<Pattern> pattern_b;
};

class RingPattern : public Pattern {
public:

    static std::shared_ptr<Pattern> make(const Color &a, const Color &b)
    {
        std::shared_ptr<Pattern> ret(new RingPattern(a,b));
        return ret;
    }
    static std::shared_ptr<Pattern> make(const Matrix &T, const Color &a, const Color &b)
    {
        std::shared_ptr<Pattern> ret(new RingPattern(T,a,b));
        return ret;
    }
    static std::shared_ptr<Pattern> make(const std::shared_ptr<Pattern> &a, const std::shared_ptr<Pattern> &b)
    {
        std::shared_ptr<Pattern> ret(new RingPattern(a,b));
        return ret;
    }
    static std::shared_ptr<Pattern> make(const Matrix &T,
                                         const std::shared_ptr<Pattern> &a,
                                         const std::shared_ptr<Pattern> &b)
    {
        std::shared_ptr<Pattern> ret(new RingPattern(T,a,b));
        return ret;
    }

    Color patternAt(const Point &pp) const override;

private:
    RingPattern(const Color &a, const Color &b) : Pattern(),
                                                    pattern_a(SolidPattern::make(a)),
                                                    pattern_b(SolidPattern::make(b)) { }
    RingPattern(const Matrix &T, const Color &a, const Color &b) : Pattern(T),
                                                                     pattern_a(SolidPattern::make(a)),
                                                                     pattern_b(SolidPattern::make(b)) { }
    RingPattern(const std::shared_ptr<Pattern> &a, const std::shared_ptr<Pattern> &b) :
            Pattern(), pattern_a(a), pattern_b(b) { }
    RingPattern(const Matrix &T, const std::shared_ptr<Pattern> &a, const std::shared_ptr<Pattern> &b) :
            Pattern(T), pattern_a(a), pattern_b(b) { }

    std::shared_ptr<Pattern> pattern_a;
    std::shared_ptr<Pattern> pattern_b;
};

class CheckerPattern : public Pattern {
public:

    static std::shared_ptr<Pattern> make(const Color &a, const Color &b)
    {
        std::shared_ptr<Pattern> ret(new CheckerPattern(a,b));
        return ret;
    }
    static std::shared_ptr<Pattern> make(const Matrix &T, const Color &a, const Color &b)
    {
        std::shared_ptr<Pattern> ret(new CheckerPattern(T,a,b));
        return ret;
    }
    static std::shared_ptr<Pattern> make(const std::shared_ptr<Pattern> &a, const std::shared_ptr<Pattern> &b)
    {
        std::shared_ptr<Pattern> ret(new CheckerPattern(a,b));
        return ret;
    }
    static std::shared_ptr<Pattern> make(const Matrix &T,
                                         const std::shared_ptr<Pattern> &a,
                                         const std::shared_ptr<Pattern> &b)
    {
        std::shared_ptr<Pattern> ret(new CheckerPattern(T,a,b));
        return ret;
    }

    Color patternAt(const Point &pp) const override;

private:
    CheckerPattern(const Color &a, const Color &b) : Pattern(),
                                                    pattern_a(SolidPattern::make(a)),
                                                    pattern_b(SolidPattern::make(b)) { }
    CheckerPattern(const Matrix &T, const Color &a, const Color &b) : Pattern(T),
                                                                     pattern_a(SolidPattern::make(a)),
                                                                     pattern_b(SolidPattern::make(b)) { }
    CheckerPattern(const std::shared_ptr<Pattern> &a, const std::shared_ptr<Pattern> &b) :
            Pattern(), pattern_a(a), pattern_b(b) { }
    CheckerPattern(const Matrix &T, const std::shared_ptr<Pattern> &a, const std::shared_ptr<Pattern> &b) :
            Pattern(T), pattern_a(a), pattern_b(b) { }

    std::shared_ptr<Pattern> pattern_a;
    std::shared_ptr<Pattern> pattern_b;
};


// Simple grid patterns for development and scene composition.
// NOTE: These patterns are based on world space coordinates and thus require
// a pointer to their parent object in order to determine their world space position.
class XZGridPattern : public Pattern {
public:

    static std::shared_ptr<Pattern> make(const std::shared_ptr<Shape> &obj)
    {
        std::shared_ptr<Pattern> ret(new XZGridPattern(obj));
        return ret;
    }

    Color patternAt(const Point &pp) const override;
    std::shared_ptr<Shape> shape;

private:
    XZGridPattern(const std::shared_ptr<Shape> &obj) : Pattern(), shape(obj) { }
};

class YZGridPattern : public Pattern {
public:

    static std::shared_ptr<Pattern> make(const std::shared_ptr<Shape> &obj)
    {
        std::shared_ptr<Pattern> ret(new YZGridPattern(obj));
        return ret;
    }

    Color patternAt(const Point &pp) const override;
    std::shared_ptr<Shape> shape;

private:
    YZGridPattern(const std::shared_ptr<Shape> &obj) : Pattern(), shape(obj) { }
};

class XYGridPattern : public Pattern {
public:

    static std::shared_ptr<Pattern> make(const std::shared_ptr<Shape> &obj)
    {
        std::shared_ptr<Pattern> ret(new XYGridPattern(obj));
        return ret;
    }

    Color patternAt(const Point &pp) const override;
    std::shared_ptr<Shape> shape;

private:
    XYGridPattern(const std::shared_ptr<Shape> &obj) : Pattern(), shape(obj) { }
};

class TestPattern : public Pattern {
public:
    static std::shared_ptr<Pattern> make()
    {
        std::shared_ptr<Pattern> ret(new TestPattern());
        return ret;
    }

    Color patternAt(const Point &pp) const override;
private:
    TestPattern() : Pattern() { }
};

class TextureMapPattern: public Pattern {
public:
    static std::shared_ptr<Pattern> make(std::shared_ptr<UVPattern> uvp, std::function<UVPoint(const Point&)> map_fcn)
    {
        std::shared_ptr<Pattern> ret(new TextureMapPattern(uvp, map_fcn));
        return ret;
    }
    static std::shared_ptr<Pattern> make(const Matrix &T, std::shared_ptr<UVPattern> uvp, std::function<UVPoint(const Point&)> map_fcn)
    {
        std::shared_ptr<Pattern> ret(new TextureMapPattern(T, uvp, map_fcn));
        return ret;
    }

    Color patternAt(const Point &pp) const override;

private:
    TextureMapPattern(std::shared_ptr<UVPattern> uvp, std::function<UVPoint(const Point&)> map_fcn) : Pattern(),
                                                                                            _uvp(uvp),
                                                                                            _map_fcn(map_fcn)
                                                                                            { }
    TextureMapPattern(const Matrix &T, std::shared_ptr<UVPattern> uvp, std::function<UVPoint(const Point&)> map_fcn) : Pattern(T),
                                                                                            _uvp(uvp),
                                                                                            _map_fcn(map_fcn)
                                                                                            { }
    std::shared_ptr<UVPattern> _uvp;
    std::function<UVPoint(const Point&)> _map_fcn;
};

class CubeMapPattern: public Pattern {
public:
    static std::shared_ptr<Pattern> make(std::shared_ptr<UVPattern> uvp_left, std::shared_ptr<UVPattern> uvp_right,
                                         std::shared_ptr<UVPattern> uvp_front, std::shared_ptr<UVPattern> uvp_back,
                                         std::shared_ptr<UVPattern> uvp_up, std::shared_ptr<UVPattern> uvp_down)
    {
        std::shared_ptr<Pattern> ret(new CubeMapPattern(uvp_left,uvp_right,uvp_front,uvp_back,uvp_up,uvp_down));
        return ret;
    }
    static std::shared_ptr<Pattern> make(const Matrix &T, std::shared_ptr<UVPattern> uvp_left, std::shared_ptr<UVPattern> uvp_right,
                                         std::shared_ptr<UVPattern> uvp_front, std::shared_ptr<UVPattern> uvp_back,
                                         std::shared_ptr<UVPattern> uvp_up, std::shared_ptr<UVPattern> uvp_down)
    {
        std::shared_ptr<Pattern> ret(new CubeMapPattern(T, uvp_left,uvp_right,uvp_front,uvp_back,uvp_up,uvp_down));
        return ret;
    }

    Color patternAt(const Point &pp) const override;

private:
    CubeMapPattern(std::shared_ptr<UVPattern> uvp_left, std::shared_ptr<UVPattern> uvp_right,
                   std::shared_ptr<UVPattern> uvp_front, std::shared_ptr<UVPattern> uvp_back,
                   std::shared_ptr<UVPattern> uvp_up, std::shared_ptr<UVPattern> uvp_down) : Pattern(),
                                                                                            _uvp_left(uvp_left),
                                                                                            _uvp_right(uvp_right),
                                                                                            _uvp_front(uvp_front),
                                                                                            _uvp_back(uvp_back),
                                                                                            _uvp_up(uvp_up),
                                                                                            _uvp_down(uvp_down)
                                                                                            { }
    CubeMapPattern(const Matrix &T, std::shared_ptr<UVPattern> uvp_left, std::shared_ptr<UVPattern> uvp_right,
                   std::shared_ptr<UVPattern> uvp_front, std::shared_ptr<UVPattern> uvp_back,
                   std::shared_ptr<UVPattern> uvp_up, std::shared_ptr<UVPattern> uvp_down) : Pattern(T),
                                                                                            _uvp_left(uvp_left),
                                                                                            _uvp_right(uvp_right),
                                                                                            _uvp_front(uvp_front),
                                                                                            _uvp_back(uvp_back),
                                                                                            _uvp_up(uvp_up),
                                                                                            _uvp_down(uvp_down)
                                                                                            { }
    std::shared_ptr<UVPattern> _uvp_left;
    std::shared_ptr<UVPattern> _uvp_right;
    std::shared_ptr<UVPattern> _uvp_front;
    std::shared_ptr<UVPattern> _uvp_back;
    std::shared_ptr<UVPattern> _uvp_up;
    std::shared_ptr<UVPattern> _uvp_down;
};