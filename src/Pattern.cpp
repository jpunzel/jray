#include "Pattern.h"
#include "Shape.h"
#include <math.h>
#define GRID_LINE_HALFWIDTH 0.035


Color Pattern::patternAtShape(const std::shared_ptr<Shape> &sp, const Point &p) const
{
    //Point obj_p = sp->getInverseTransform() * p;
    Point obj_p = sp->world_to_object(p);
    Point pattern_p = inverse_transform * obj_p;

    return patternAt(pattern_p);
}

Color SolidPattern::patternAt(const Point &pp) const
{
    return color;
}

Color BlendedPattern::patternAt(const Point &pp) const
{
    Color color_a = pattern_a->patternAt(pattern_a->getTransform() * pp);
    Color color_b = pattern_b->patternAt(pattern_b->getTransform() * pp);
    return color_a + color_b;
}

Color StripePattern::patternAt(const Point &pp) const
{
    if ( fmod(floor(pp.x()), 2) == 0) {
        return pattern_a->patternAt(pattern_a->getTransform() * pp);
    }
    return pattern_b->patternAt(pattern_b->getTransform() * pp);
}

Color GradientPattern::patternAt(const Point &pp) const
{
    Color color_a = pattern_a->patternAt(pattern_a->getTransform() * pp);
    Color color_b = pattern_b->patternAt(pattern_b->getTransform() * pp);
    Color diff = color_b - color_a;
    double frac = pp.x() - floor(pp.x());
    return color_a + diff * frac;
}

Color RingPattern::patternAt(const Point &pp) const
{
    double distance = sqrt( pp.x()*pp.x() + pp.z()*pp.z() );
    if ( fmod( floor(distance) , 2 ) == 0 ) {
        return pattern_a->patternAt(pattern_a->getTransform() * pp);
    }
    return pattern_b->patternAt(pattern_b->getTransform() * pp);
}

Color CheckerPattern::patternAt(const Point &pp) const
{
    if ( fmod( floor(pp.x()) + floor(pp.y()) + floor(pp.z()) , 2) == 0 ) {
        return pattern_a->patternAt(pattern_a->getTransform() * pp);
    }
    return pattern_b->patternAt(pattern_b->getTransform() * pp);
}

Color XZGridPattern::patternAt(const Point &pp) const
{
    // transform pattern point back into object space
    Point obj_p = transform * pp;
    // transform object point back into world space
    Point world_p = shape->getTransform() * obj_p;
    if ( world_p.x() <= (floor(world_p.x()) + GRID_LINE_HALFWIDTH) || 
         world_p.x() >= (ceil(world_p.x()) - GRID_LINE_HALFWIDTH)  ||
         world_p.z() <= (floor(world_p.z()) + GRID_LINE_HALFWIDTH) ||
         world_p.z() >= (ceil(world_p.z()) - GRID_LINE_HALFWIDTH) ) {
             return Color::Black;
         }
    else {
        return Color::White;
    }
}
Color YZGridPattern::patternAt(const Point &pp) const
{
    // transform pattern point back into object space
    Point obj_p = transform * pp;
    // transform object point back into world space
    Point world_p = shape->getTransform() * obj_p;
    if ( world_p.y() <= (floor(world_p.y()) + GRID_LINE_HALFWIDTH) || 
         world_p.y() >= (ceil(world_p.y()) - GRID_LINE_HALFWIDTH)  ||
         world_p.z() <= (floor(world_p.z()) + GRID_LINE_HALFWIDTH) ||
         world_p.z() >= (ceil(world_p.z()) - GRID_LINE_HALFWIDTH) ) {
             return Color::Black;
         }
    else {
        return Color::White;
    }
}
Color XYGridPattern::patternAt(const Point &pp) const
{
    // transform pattern point back into object space
    Point obj_p = transform * pp;
    // transform object point back into world space
    Point world_p = shape->getTransform() * obj_p;
    if ( world_p.x() <= (floor(world_p.x()) + GRID_LINE_HALFWIDTH) || 
         world_p.x() >= (ceil(world_p.x()) - GRID_LINE_HALFWIDTH)  ||
         world_p.y() <= (floor(world_p.y()) + GRID_LINE_HALFWIDTH) ||
         world_p.y() >= (ceil(world_p.y()) - GRID_LINE_HALFWIDTH) ) {
             return Color::Black;
         }
    else {
        return Color::White;
    }
}

Color TestPattern::patternAt(const Point &pp) const
{
    return Color(pp.x(), pp.y(), pp.z());
}

Color TextureMapPattern::patternAt(const Point &pp) const
{
    UVPoint p = _map_fcn(pp);
    return _uvp->uv_patternAt(p);
}

Color CubeMapPattern::patternAt(const Point &pp) const
{
    unsigned short face = UVPattern::cubeFaceFromPoint(pp);
    if (face == CUBE_FACE_LEFT) {
        return _uvp_left->uv_patternAt( UVPattern::cubeMapLeft(pp) );
    }
    if (face == CUBE_FACE_RIGHT ) {
        return _uvp_right->uv_patternAt( UVPattern::cubeMapRight(pp) );
    }
    if (face == CUBE_FACE_FRONT ) {
        return _uvp_front->uv_patternAt( UVPattern::cubeMapFront(pp) );
    }
    if (face == CUBE_FACE_BACK ) {
        return _uvp_back->uv_patternAt( UVPattern::cubeMapBack(pp) );
    }
    if (face == CUBE_FACE_UP ) {
        return _uvp_up->uv_patternAt( UVPattern::cubeMapUp(pp) );
    }
    return _uvp_down->uv_patternAt( UVPattern::cubeMapDown(pp) );
}