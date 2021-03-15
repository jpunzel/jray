#include "Canvas.h"

// Creates an array of N=height pointers, each one pointing at the start of a pixel row
// in the given GdkPixbuf.
pixel** Canvas::init_pixbuf_data_ptrs(const Glib::RefPtr<Gdk::Pixbuf> &pb, size_t width, size_t height)
{
    unsigned char *pixel_byte_ptr = pb->get_pixels();
    int rowstride = pb->get_rowstride();
    pixel **pbdata = new pixel*[height];
    for (size_t i = 0; i < height; ++i, pixel_byte_ptr += rowstride) {
        pbdata[i] = (pixel*) pixel_byte_ptr;
    }
    return pbdata;
}

Canvas::Canvas(int w, int h ) :
    width(w), height(h)
{
    pixbuf = Gdk::Pixbuf::create(Gdk::COLORSPACE_RGB, false, 8, width, height);
    pixbuf_data = init_pixbuf_data_ptrs(pixbuf, width, height);
}

Canvas::Canvas(const std::string &filename)
{
    pixbuf = Gdk::Pixbuf::create_from_file(filename);
    width = pixbuf->get_width();
    height = pixbuf->get_height();
    pixbuf_data = init_pixbuf_data_ptrs(pixbuf, width, height);
}

Canvas::Canvas(const Canvas& obj) : width(obj.width), height(obj.height)
{
    pixbuf = Gdk::Pixbuf::create(Gdk::COLORSPACE_RGB, false, 8, width, height);
    pixbuf_data = init_pixbuf_data_ptrs(pixbuf, width, height);
    for (int y = 0; y < height; y++ ) {
        for (int x = 0; x < width; x++ ) {
            pixbuf_data[y][x] = obj.pixbuf_data[y][x];
        }
    }
}

Canvas& Canvas::operator=(const Canvas& rhs)
{
    Glib::RefPtr<Gdk::Pixbuf> pixbuf_tmp = Gdk::Pixbuf::create(Gdk::COLORSPACE_RGB, false, 8, rhs.width, rhs.height);
    pixel **pixbuf_data_tmp = init_pixbuf_data_ptrs(pixbuf_tmp, rhs.width, rhs.height);
    for (int y = 0; y < rhs.height; y++ ) {
        for (int x = 0; x < rhs.width; x++) {
            pixbuf_data_tmp[y][x] = rhs.pixbuf_data[y][x];
        }
    }
    delete [] pixbuf_data;
    pixbuf = pixbuf_tmp;
    pixbuf_data = pixbuf_data_tmp;
    width = rhs.width;
    height = rhs.height;
    // can't copy mutex, leave it to be freshly default constructed
    return *this;
}

Canvas::~Canvas()
{
    delete [] pixbuf_data;
}

void Canvas::put_pixel(int img_x, int img_y, guchar red, guchar green, guchar blue)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    if (img_x >= width || img_y >= height) {
        throw std::range_error("Pixel coordinates out of bounds");
    }
    pixel p;
    p.r = red;
    p.g = green;
    p.b = blue;
    pixbuf_data[img_y][img_x] = p;
}

void Canvas::put_pixel(Point p, Color c)
{
    put_pixel( p.x(), p.y(), c.r(), c.g(), c.b() );
}

Color Canvas::get_pixel(int img_x, int img_y) const
{
    return Color(pixbuf_data[img_y][img_x].r / 255.0,
                 pixbuf_data[img_y][img_x].g / 255.0,
                 pixbuf_data[img_y][img_x].b / 255.0 );
}

void Canvas::save(std::string filename)
{
    std::string ext = getFileExtension(filename);
    if (ext != "png" && ext != "jpeg" && ext != "jpg" && ext != "bmp") {
        throw std::invalid_argument("Bad filename extension given");
    }
    if (ext == "jpg") ext = "jpeg"; // Fix extension to match proper GdkPixbuf 'type' value
    try {
        pixbuf->save(filename, ext, {}, {});
    } catch ( std::exception &e ) {
        std::cerr << "Error saving file " << filename << ": ";
        std::cerr << e.what() << std::endl;
    }
}