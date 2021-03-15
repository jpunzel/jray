#include "BoundingBox.h"
#include "Cube.h"
#include <cmath>

const Point BoundingBox::infinityPoint = Point( std::numeric_limits<double>::infinity(),
                                                std::numeric_limits<double>::infinity(),
                                                std::numeric_limits<double>::infinity());
const Point BoundingBox::minusInfinityPoint = Point( -std::numeric_limits<double>::infinity(),
                                                     -std::numeric_limits<double>::infinity(),
                                                     -std::numeric_limits<double>::infinity());


// adjusts size of bounding box as necessary
void BoundingBox::add(const Point &pt)
{
    if ( pt.x() < min.x() )
        min[0] = pt.x();
    if ( pt.y() < min.y() )
        min[1] = pt.y();
    if ( pt.z() < min.z() )
        min[2] = pt.z();
    if ( pt.x() > max.x() )
        max[0] = pt.x();
    if ( pt.y() > max.y() )
        max[1] = pt.y();
    if ( pt.z() > max.z() )
        max[2] = pt.z();
}

void BoundingBox::add(const BoundingBox &bb)
{
    add(bb.min);
    add(bb.max);
}

bool BoundingBox::contains(const Point &pt) const
{
    return (pt.x() >= min.x() && pt.x() <= max.x() &&
            pt.y() >= min.y() && pt.y() <= max.y() &&
            pt.z() >= min.z() && pt.z() <= max.z() );
}

bool BoundingBox::contains(const BoundingBox &bb) const
{
    return ( contains(bb.min) && contains(bb.max) );
}

BoundingBox BoundingBox::transform(const Matrix &M)
{
    Point p1, p2, p3, p4, p5, p6, p7, p8;
    p1 = min;
    p2 = Point(min.x(), min.y(), max.z());
    p3 = Point(min.x(), max.y(), min.z());
    p4 = Point(min.x(), max.y(), max.z());
    p5 = Point(max.x(), min.y(), min.z());
    p6 = Point(max.x(), min.y(), max.z());
    p7 = Point(max.x(), max.y(), min.z());
    p8 = max;

    BoundingBox bb = BoundingBox();
    for (auto p : {p1, p2, p3, p4, p5, p6, p7, p8}) {
        bb.add(M * p);
    }

    return bb;
}

std::pair<BoundingBox,BoundingBox> BoundingBox::splitBounds() const
{
    double dx = abs(min.x() - max.x());
    double dy = abs(min.y() - max.y());
    double dz = abs(min.z() - max.z());
    
    double greatest = std::max({dx, dy, dz});

    double x0 = min.x();
    double y0 = min.y();
    double z0 = min.z();

    double x1 = max.x();
    double y1 = max.y();
    double z1 = max.z();

    if ( greatest == dx ) {
        x0 = x1 = x0 + dx / 2.0;
    } else if ( greatest == dy ) {
        y0 = y1 = y0 + dy / 2.0;
    } else {
        z0 = z1 = z0 + dz / 2.0;
    }

    Point mid_min = Point(x0, y0, z0);
    Point mid_max = Point(x1, y1, z1);

    return std::pair<BoundingBox,BoundingBox>(BoundingBox(min, mid_max),
                                              BoundingBox(mid_min, max) );

}


// Almost the same as the Cube localIntersect function, but we don't
// bother returning the points of intersection
bool BoundingBox::intersects(const Ray &ray) const
{
    std::pair<double,double> xtminmax, ytminmax, ztminmax;
    double xtmin, xtmax, ytmin, ytmax, ztmin, ztmax;
    double tmin, tmax;
    xtminmax = check_axis(ray.origin.x(), ray.dir.x(), min.x(), max.x() );
    ytminmax = check_axis(ray.origin.y(), ray.dir.y(), min.y(), max.y() );
    ztminmax = check_axis(ray.origin.z(), ray.dir.z(), min.z(), max.z() );

    xtmin = xtminmax.first;
    xtmax = xtminmax.second;
    ytmin = ytminmax.first;
    ytmax = ytminmax.second;
    ztmin = ztminmax.first;
    ztmax = ztminmax.second;

    tmin = std::max({xtmin, ytmin, ztmin});
    tmax = std::min({xtmax, ytmax, ztmax});

    if (tmin > tmax) { // we missed
        return false; // iset_out remains empty
    }
    return true;
}

inline std::pair<double,double> BoundingBox::check_axis(double origin, double direction, double min, double max) const
{
    double tmin_numerator = (min - origin);
    double tmax_numerator = (max - origin);
    double tmin, tmax;

    tmin = tmin_numerator / direction;
    tmax = tmax_numerator / direction;

    if ( tmin > tmax ) {
        std::swap(tmin, tmax);
    }

    return std::make_pair(tmin, tmax);
}
