#pragma once

#include <iostream>
#include <gtkmm.h>
#include <gdkmm/pixbuf.h>
#include <exception>
#include <mutex>
#include "util.h"
#include "Point.h"
#include "Color.h"

struct pixel {
    unsigned char r = 0, g = 0, b = 0;
};

class Canvas
{
public:
    Canvas(int w, int h );
    Canvas(const std::string &filename);
    Canvas(const Canvas& obj);
    Canvas& operator=(const Canvas& rhs);
    ~Canvas();

   void put_pixel(int img_x, int img_y, guchar red, guchar green, guchar blue);
   void put_pixel(Point p, Color c );
   Color get_pixel(int img_x, int img_y) const;
   int get_width() const { return width; }
   int get_height() const { return height; }
   void save(std::string filename);
   Glib::RefPtr<Gdk::Pixbuf> getPixbuf() {
       return pixbuf;
   }
private:
    int width;
    int height;

    Glib::RefPtr<Gdk::Pixbuf> pixbuf;
    pixel **pixbuf_data;
    mutable std::mutex m_mutex;

    // For initializing pixbuf_data:
    // Creates an array of N=height pointers, each one pointing at the start of a pixel row
    // in the given GdkPixbuf.
    pixel** init_pixbuf_data_ptrs(const Glib::RefPtr<Gdk::Pixbuf> &pb, size_t width, size_t height);
};