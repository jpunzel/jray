#pragma once

#include <gtkmm.h>
#include <gdkmm/pixbufloader.h>
#include <iostream>
#include "Renderer.h"
#include "SceneConfig.h"
#include <chrono>

class MainWindow : public Gtk::Window
{
public:
    MainWindow(size_t numRenderThreads, SceneConfig &config, std::string filename);
    virtual ~MainWindow();

    void on_loader_area_prepared();
    void on_loader_area_updated(int x, int y, int width, int height);
    void on_button_save_clicked();
    void start_render();
    // void on_render_complete();
    bool on_window_delete(GdkEventAny *event);
    void on_rerender_clicked();

    void reload_config();

    void notify();
    void on_render_notify();

    bool update_pixbuf();
    void kill_render();

protected:
    Gtk::Box m_VBox;
    Gtk::ButtonBox m_ButtonBox;
    Gtk::Button m_Button_Save;
    Gtk::Button m_Button_ReRender;
    Gtk::Frame m_Frame;
    Gtk::Image m_Image;
    std::string m_filename; // hold on to this for reloading config to re-render
    Renderer m_renderer;
    size_t m_threads;
    std::chrono::time_point<std::chrono::steady_clock> m_render_start;

    sigc::connection m_TimeoutHandler;
    Glib::Dispatcher m_Dispatcher;
    std::thread* m_RenderThread;

    // The config object is passed to the constructor, and lasts for the duration of the program.
    // I'm OK with this being a reference member because we don't need operator= for MainWindow.
    SceneConfig &config;
};