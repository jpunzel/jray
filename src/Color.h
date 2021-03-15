#pragma once
#include "Tuple.h"

class Color: public Tuple
{
public:
    Color() {
        data[0] = data[1] = data[2] = 0;
        data[3] = 0;
    }
    Color(double r, double g, double b) {
        // Note, range is [0,1] but we don't clamp values until we output RGB.
        // This saves a modest amount of time since many of our colors are only
        // used in intermediate calculations for blending and aren't outputted.
        data[0] = r;
        data[1] = g;
        data[2] = b;
        data[3] = 0;
    }
    // Derived-to-base conversion
    Color(const Tuple &t) {
        data[0] = t.x();
        data[1] = t.y();
        data[2] = t.z();
        data[3] = 0;
    }

    static const Color Black;
    static const Color White;
    static const Color Red;
    static const Color Green;
    static const Color Blue;

    friend bool operator==(const Color &c1, const Color &c2)
    {
        return ( doubleEqual( clamp_unit_interval(c1.data[0]), clamp_unit_interval(c2.data[0]) ) &&
                 doubleEqual( clamp_unit_interval(c1.data[1]), clamp_unit_interval(c2.data[1]) ) &&
                 doubleEqual( clamp_unit_interval(c1.data[2]), clamp_unit_interval(c2.data[2]) ) &&
                 doubleEqual( clamp_unit_interval(c1.data[3]), clamp_unit_interval(c2.data[3]) ) );
    }
    friend bool operator!=(const Color &c1, const Color &c2)
    {
        return !(c1 == c2);
    }

    // Ensure we don't call functions that don't make sense on Colors
    Tuple translate(double x, double y, double z) = delete;
    Tuple scale(double x, double y, double z) = delete;
    Tuple rotate_x(double radians) = delete;
    Tuple rotate_y(double radians) = delete;
    Tuple rotate_z(double radians) = delete;
    Tuple shear(double x_y, double x_z, double y_x, double y_z, double z_x, double z_y) = delete;

    // These member functions will be used when outputting color channel values,
    // so we finally cap the floating point values at 1.0 here to prevent overflow.
    unsigned char r() const { 
        double r = (data[0] > 1.0) ? 1.0 : data[0];
        return static_cast<unsigned char>(r * 255.0);
    }
    unsigned char g() const {
        double g = (data[1] > 1.0) ? 1.0 : data[1];
        return static_cast<unsigned char>(g * 255.0);
    }
    unsigned char b() const {
        double b = (data[2] > 1.0) ? 1.0 : data[2];
        return static_cast<unsigned char>(b * 255.0);
    }

private:
};
