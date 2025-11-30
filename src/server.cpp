#include "server.h"
#include <iostream>

namespace imgstore {

Server::Server(const std::string& storageDir, int port)
    : port_(port),
      storage_(std::make_shared<StorageManager>(storageDir)),
      handler_(std::make_shared<ImageHandler>(storage_)) {
    setupRoutes();
}

void Server::setupRoutes() {
    // Health check endpoint
    CROW_ROUTE(app_, "/health")
    ([this]() {
        return handler_->handleHealth();
    });

    // Upload endpoint - POST /images
    CROW_ROUTE(app_, "/images").methods(crow::HTTPMethod::POST)
    ([this](const crow::request& req) {
        return handler_->handleUpload(req);
    });

    // Download endpoint - GET /images/<id>
    CROW_ROUTE(app_, "/images/<string>")
    ([this](const std::string& imageId) {
        return handler_->handleDownload(imageId);
    });

    // Delete endpoint - DELETE /images/<id>
    CROW_ROUTE(app_, "/images/<string>").methods(crow::HTTPMethod::DELETE)
    ([this](const std::string& imageId) {
        return handler_->handleDelete(imageId);
    });
}

void Server::run() {
    std::cout << "Starting image storage server on port " << port_ << std::endl;
    std::cout << "Storage directory: " << storage_->getImagePath("").parent_path() << std::endl;
    std::cout << "\nAvailable endpoints:" << std::endl;
    std::cout << "  POST   /images       - Upload image" << std::endl;
    std::cout << "  GET    /images/<id>  - Download image" << std::endl;
    std::cout << "  DELETE /images/<id>  - Delete image" << std::endl;
    std::cout << "  GET    /health       - Health check" << std::endl;
    std::cout << std::endl;

    app_.port(port_).multithreaded().run();
}

void Server::stop() {
    app_.stop();
}

} // namespace imgstore
