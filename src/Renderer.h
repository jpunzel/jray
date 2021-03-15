#pragma once
#include <gtkmm.h>
#include <gdkmm/pixbuf.h>
#include "Canvas.h"
#include "Color.h"
#include "Pattern.h"
#include "Camera.h"
#include "World.h"
#include "Sphere.h"
#include "Plane.h"
#include "SceneConfig.h"
#include <thread>
#include <mutex>
#include <math.h>
#define PI 3.1415926535898

// Forward declaration of MainWindow
class MainWindow;

class Renderer 
{
public:

    Renderer(size_t threads, const SceneConfig &config) :
                                      m_killrender(false),
                                      width(config.getWidth()),
                                      height(config.getHeight()),
                                      m_camera(config.getCamera()),
                                      m_canvas(Canvas(config.getWidth(), config.getHeight())),
                                      m_world(config.getWorld()),
                                      numThreads(threads) { }


    void kill_render();

    void render_pixel_row(int y);
    void render_portion(size_t threadnum);
    void render(MainWindow* caller);

    Glib::RefPtr<Gdk::Pixbuf> getPixbuf() {
        return m_canvas.getPixbuf();
    }

    Canvas& getCanvas();

private:
    bool m_killrender;
    size_t width;
    size_t height;
    Camera m_camera;
    Canvas m_canvas;
    World m_world;
    size_t numThreads;
};