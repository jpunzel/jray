#include "MainWindow.h"

MainWindow::MainWindow(size_t numRenderThreads, SceneConfig &config, std::string filename) 
                         :  m_VBox(Gtk::ORIENTATION_VERTICAL, 8),
                            m_ButtonBox(Gtk::ORIENTATION_VERTICAL),
                            m_Button_Save("Save to file"),
                            m_Button_ReRender("Re-render"),
                            m_filename(filename),
                            m_renderer(numRenderThreads, config),
                            m_threads(numRenderThreads),
                            m_Dispatcher(),
                            m_RenderThread(nullptr),
                            config(config)
{
    signal_delete_event().connect(sigc::mem_fun(*this, &MainWindow::on_window_delete ) );

    set_title("Jared's Raytracer");
    set_size_request(config.getWidth(), config.getHeight());

    set_border_width(10);

    m_VBox.set_border_width(10);
    add(m_VBox);

    m_VBox.add(m_ButtonBox);
    m_ButtonBox.pack_start(m_Button_Save);
    m_Button_Save.signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::on_button_save_clicked) );
    m_Button_Save.set_sensitive(false); // Disable button until render finishes

    m_ButtonBox.pack_end(m_Button_ReRender);
    m_Button_ReRender.signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::on_rerender_clicked));

    m_Frame.set_shadow_type(Gtk::SHADOW_ETCHED_OUT);
    m_Frame.set_halign(Gtk::ALIGN_CENTER);
    m_Frame.set_valign(Gtk::ALIGN_CENTER);
    m_VBox.pack_start(m_Frame, Gtk::PACK_SHRINK);

    m_Frame.add(m_Image);

    m_Dispatcher.connect(sigc::mem_fun(*this, &MainWindow::on_render_notify));

    // fill image with gray pixels as a placeholder
    auto refPixbuf = m_renderer.getPixbuf();
    refPixbuf->fill(0xaaaaaaff);
    m_Image.set(refPixbuf);

    show_all();
    start_render();

}

MainWindow::~MainWindow()
{

}

void MainWindow::on_rerender_clicked()
{
    m_Button_ReRender.set_sensitive(false);
    m_Button_Save.set_sensitive(false); 
    kill_render();
    reload_config();
    
    auto refPixbuf = m_renderer.getPixbuf();
    refPixbuf->fill(0xaaaaaaff);
    m_Image.set(refPixbuf);

    start_render();

}

void MainWindow::reload_config()
{
    config = SceneConfig(m_filename);
    m_renderer = Renderer(m_threads, config);
    set_size_request(config.getWidth(), config.getHeight());
    resize(config.getWidth(), config.getHeight());
}


void MainWindow::start_render()
{

    m_render_start = std::chrono::steady_clock::now();
    m_TimeoutHandler = Glib::signal_timeout().connect(sigc::mem_fun(*this, &MainWindow::update_pixbuf), 10);

    if (m_RenderThread) {
        std::cout << "Cannot start a render thread while another one is running." << std::endl;
    } else {
        m_RenderThread = new std::thread(
            [this]
            {
                m_renderer.render(this);
            });
    }

    m_Button_ReRender.set_sensitive(true); // Ok to re-enable button

}

bool MainWindow::update_pixbuf()
{
    auto refPixbuf = m_renderer.getPixbuf();
    m_Image.set(refPixbuf);
    if (m_RenderThread) {
        return true;
    }
    return false;
}

void MainWindow::notify()
{
    m_Dispatcher.emit();
}

void MainWindow::on_render_notify()
{
    if ( m_RenderThread && m_RenderThread->joinable())
        m_RenderThread->join();
    
    delete m_RenderThread;
    m_RenderThread = nullptr;

    double duration = (std::chrono::duration_cast<std::chrono::milliseconds>
                      (std::chrono::steady_clock::now() - m_render_start)  ).count() / 1000.0;
    std::cout << "Render completed in " << duration << " seconds." << std::endl;
    m_Button_Save.set_sensitive(true);
}

void MainWindow::on_button_save_clicked()
{
    Gtk::FileChooserDialog dialog("Save to...", Gtk::FILE_CHOOSER_ACTION_SAVE);
    dialog.set_transient_for(*this);

    dialog.add_button("_Cancel", Gtk::RESPONSE_CANCEL);
    dialog.add_button("_Save", Gtk::RESPONSE_OK);

    auto filter_images = Gtk::FileFilter::create();
    filter_images->set_name("Image files");
    filter_images->add_mime_type("image/jpeg");
    filter_images->add_mime_type("image/png");
    filter_images->add_mime_type("image/bmp");
    dialog.add_filter(filter_images);

    dialog.set_current_name("image.png");

    
    int result = dialog.run();
    std::string filename;
    std::string fileext;

    switch (result)
    {
        case(Gtk::RESPONSE_OK):
            filename = dialog.get_filename();
            fileext = getFileExtension(filename);
            if (fileext != "jpg" && fileext != "jpeg" && fileext != "png" && fileext != "bmp") {
                Gtk::MessageDialog errorDialog(dialog, "Invalid file extension");
                errorDialog.set_secondary_text("Please specify a supported image file extension (jpg, bmp, png)");
                errorDialog.set_transient_for(dialog);
                errorDialog.run();
                break;
            }
            m_renderer.getCanvas().save(filename);
            break;
        case(Gtk::RESPONSE_CANCEL):
            break;
        default:
            break;
    }

}

void MainWindow::kill_render()
{
    m_renderer.kill_render();
    if ( m_RenderThread && m_RenderThread->joinable() ) {
        m_RenderThread->join();
        std::cout << "Existing render thread killed." << std::endl;
    }
    delete m_RenderThread;
    m_RenderThread = nullptr;
    m_TimeoutHandler.disconnect(); // Don't risk waiting several milliseconds for the timeout
                                   // thread to return false to be disconnected. Do it now.
}

bool MainWindow::on_window_delete(GdkEventAny *event)
{
    kill_render();
    return false;
}


