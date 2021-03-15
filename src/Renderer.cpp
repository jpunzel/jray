#include <random>
#include "Renderer.h"
#include "MainWindow.h"

Canvas& Renderer::getCanvas() {
    return m_canvas;
}

void Renderer::render(MainWindow *caller)
{
    m_killrender = false;

    std::vector<std::thread> threads;

    for (size_t i = 0; i < numThreads; i++) {
        threads.push_back( std::thread(&Renderer::render_portion, this, i) );
        std::cout << "Started thread " << i << std::endl;
    }

    for (auto &th: threads) {
        if (th.joinable()) th.join();
    }

    // Do not notify if:
    // 1.) If caller is null, we are not in GUI mode and there is no window to notify.
    // 2.) We've been killed, otherwise this will trigger
    //     the MainWindow notify function to cleanup and kill any newer threads.
    if (caller && ! m_killrender)
        caller->notify();
}

void Renderer::render_portion(size_t threadnum)
{
    for (size_t y = 0; y < height; y++) {
        if (m_killrender) return;
        if (y % numThreads == threadnum) {
            render_pixel_row(y);
        }
    }
}

void Renderer::render_pixel_row(int y)
{
    size_t samples = m_camera.getSupersamplingLevel();
    size_t focal_samples = m_camera.getFocalSamples();

    Iset iset;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0, 1);

    for (size_t x = 0; x < width; x++) {
        if (m_killrender) break;
        Color final_color;
        Color focal_color;
        double px_offset, py_offset;

        for (size_t i = 0; i < samples; i++) {
            if ( samples == 1 ) {
                px_offset = py_offset = 0.5;
            } else {
                px_offset = dis(gen);
                py_offset = dis(gen);
            }
            // implement focal blur by taking multiple samples with same pixel offset
            focal_color = Color(0,0,0);
            for (size_t j = 0; j < focal_samples; j++) {
                Ray r = m_camera.ray_for_pixel(x, y, px_offset, py_offset);
                Color c = m_world.colorAt(r, iset);
                iset.clear();
                focal_color += c;
            }
            focal_color /= focal_samples;
            final_color += focal_color;
        }

        final_color /= samples;
        
        Point px = Point(x,y,0);
        m_canvas.put_pixel(px, final_color);
    }
}

void Renderer::kill_render()
{
    m_killrender = true;
}