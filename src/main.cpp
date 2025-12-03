#include "server.h"
#include <iostream>
#include <string>
#include <cstdlib>

int main(int argc, char* argv[]) {
    // Default configuration
    std::string storageDir = "./storage";
    int port = 8080;
    std::string apiKey;

    // Check for API key in environment variable
    if (const char* envApiKey = std::getenv("IMG_STORE_API_KEY")) {
        apiKey = envApiKey;
    }

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
        } else if (arg == "--api-key" || arg == "-k") {
            if (i + 1 < argc) {
                apiKey = argv[++i];
            }
        } else if (arg == "--help" || arg == "-h") {
            std::cout << "Usage: " << argv[0] << " [OPTIONS]" << std::endl;
            std::cout << "\nOptions:" << std::endl;
            std::cout << "  -p, --port <port>        Server port (default: 8080)" << std::endl;
            std::cout << "  -s, --storage <dir>      Storage directory (default: ./storage)" << std::endl;
            std::cout << "  -k, --api-key <key>      API key for write operations" << std::endl;
            std::cout << "  -h, --help               Show this help message" << std::endl;
            std::cout << "\nEnvironment Variables:" << std::endl;
            std::cout << "  IMG_STORE_API_KEY        API key (alternative to --api-key)" << std::endl;
            std::cout << "\nSecurity:" << std::endl;
            std::cout << "  • GET requests are always public (no authentication)" << std::endl;
            std::cout << "  • POST/DELETE require API key via:" << std::endl;
            std::cout << "    - Authorization: Bearer <key> header" << std::endl;
            std::cout << "    - X-API-Key: <key> header" << std::endl;
            std::cout << "  • If no API key is set, all endpoints are public (testing only)" << std::endl;
            return 0;
        }
    }

    try {
        imgstore::Server server(storageDir, port, apiKey);
        server.run();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
