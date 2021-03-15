#include <iostream>
#include <gtkmm.h>
#include <vector>
#include "MainWindow.h"
#include "Renderer.h"
#include "SceneConfig.h"
#include <getopt.h>
#include <sys/stat.h>

void print_usage(const std::string &binname)
{
    std::cout << "Usage: " << binname << " [-h] [-t THREADS] [-o OUTPUT_IMAGE_FILE] [scene file]" << std::endl;
    std::cout <<
    "Options:\n"
    "   -o, --output     :   Disables GUI and outputs image to specified file.\n"
    "                        Filename must have extension .png, .jpg, or .bmp\n"
    "   -t, --threads    :   Specifies number of rendering threads to be used.\n"
    "                        Default: number of CPUs present on this machine\n"
    "   -d  --dir        :   Specifies working directory (for scene files and output images)\n"
    "                        Default: Current directory (.)\n"
    "   -h, --help       :   Print this help message\n"
    "Arguments:\n"
    "   scene file       :   Specifies the scene description YAML file to be rendered\n"
    "                        Default: scene.yaml\n"
    << std::endl;
    exit(EXIT_SUCCESS);
}

bool file_exists(const std::string &filename)
{
    struct stat buf;
    return (stat(filename.c_str(), &buf) == 0);
}

int main(int argc, char** argv)
{
	size_t threads = std::thread::hardware_concurrency();
    std::string scenefile;
    std::string cwd;
    std::string output_imgfile = "";
    int c;
    static struct option long_options[] = {
        {"output", required_argument, nullptr, 'o'},
        {"threads", required_argument, nullptr, 't'},
        {"dir", required_argument, nullptr, 'd'},
        {"help", no_argument, nullptr, 'h'},
        { nullptr, no_argument, nullptr, 0 }
    };

    while ( true ) {
        c = getopt_long(argc, argv, "o:t:d:h", long_options, nullptr);
        if (c == -1)
            break;
        
        switch(c) {
            case 'o':
                output_imgfile = optarg;
                break;
            case 't':
                threads = std::stoi(optarg);
                break;
            case 'd':
                cwd = optarg;
                break;
            case 'h':
            case '?':
            default:
                print_usage(argv[0]);
                break;
        }
    }

    if ( chdir(cwd.c_str()) != 0 ) {
        perror("Cannot chdir");
        exit(EXIT_FAILURE);
    }

    if (optind < argc) {
        scenefile = argv[optind++];
        if (optind < argc) {
            std::cerr << "Too many arguments given." << std::endl;
            print_usage(argv[0]);
        }
    } else {
        std::cerr << "No scene description file given. Defaulting to scene.yaml" << std::endl;
        scenefile = "scene.yaml";
    }

    // Catch the most common error of a missing scene file.
    // All other file errors will be found by the yaml parser.
    if ( !file_exists(scenefile) ) {
        std::cerr << "Error: Scene file '" << scenefile << "' not found." << std::endl;
        exit(EXIT_FAILURE);
    }

    //std::cout << "output: " << output_imgfile << std::endl;
    //std::cout << "threads: " << threads << std::endl;
    //std::cout << "scenefile: " << scenefile << std::endl;

    // Create our Gtk application regardless of whether we display a window.
    // We need the Gtk context created to construct a GdkPixbuf.
    auto app = Gtk::Application::create("com.imjared.raytracer", Gio::APPLICATION_NON_UNIQUE);

	SceneConfig config = SceneConfig(scenefile);

    // If we're given an output file, render image and exit without displaying our main window.
    if ( output_imgfile != "" ) {
        std::string fileext = getFileExtension(output_imgfile);
        if (fileext != "jpg" && fileext != "jpeg" && fileext != "png" && fileext != "bmp") {
            std::cerr << "Output file must have one of these extensions: .jpg, .jpeg, .png, .bmp" << std::endl;
            exit(EXIT_FAILURE);
        }
	    auto renderer = new Renderer(threads, config);
	    renderer->render(nullptr);
	    renderer->getCanvas().save(output_imgfile);
    }
    // Otherwise, just run the application in the main window.
    else {
        MainWindow window(threads, config, scenefile);
        return app->run(window);
    }


}
