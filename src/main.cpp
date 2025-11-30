#include "server.h"
#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
    // Default configuration
    std::string storageDir = "./storage";
    int port = 8080;

    // Parse command-line arguments
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--port" || arg == "-p") {
            if (i + 1 < argc) {
                port = std::stoi(argv[++i]);
            }
        } else if (arg == "--storage" || arg == "-s") {
            if (i + 1 < argc) {
                storageDir = argv[++i];
            }
        } else if (arg == "--help" || arg == "-h") {
            std::cout << "Usage: " << argv[0] << " [OPTIONS]" << std::endl;
            std::cout << "\nOptions:" << std::endl;
            std::cout << "  -p, --port <port>        Server port (default: 8080)" << std::endl;
            std::cout << "  -s, --storage <dir>      Storage directory (default: ./storage)" << std::endl;
            std::cout << "  -h, --help               Show this help message" << std::endl;
            return 0;
        }
    }

    try {
        imgstore::Server server(storageDir, port);
        server.run();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
