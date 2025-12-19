#include "server.h"
#include <iostream>

namespace imgstore {

Server::Server(const std::string& storageDir, int port, const std::string& apiKey)
    : port_(port),
      storage_(std::make_shared<StorageManager>(storageDir)),
      handler_(std::make_shared<ImageHandler>(storage_)),
      authEnabled_(!apiKey.empty()) {
    
    if (authEnabled_) {
        auth_ = std::make_shared<AuthMiddleware>(apiKey);
        std::cout << "🔒 API key authentication enabled" << std::endl;
        std::cout << "   GET requests: Public (no auth required)" << std::endl;
        std::cout << "   POST/DELETE: Protected (API key required)" << std::endl;
    } else {
        std::cout << "⚠️  WARNING: Running without authentication!" << std::endl;
        std::cout << "   All endpoints are publicly accessible." << std::endl;
        std::cout << "   Set IMG_STORE_API_KEY environment variable to enable auth." << std::endl;
    }
    
    setupRoutes();
}

bool Server::requireAuth(const crow::request& req) {
    if (!authEnabled_) {
        return true; // Auth disabled, allow all
    }
    
    // Check Authorization header first
    auto authHeader = req.get_header_value("Authorization");
    if (authHeader.empty()) {
        // Fallback to X-API-Key header
        authHeader = req.get_header_value("X-API-Key");
    }
    
    auto apiKey = auth_->extractApiKey(authHeader);
    if (!apiKey || !auth_->validateApiKey(*apiKey)) {
        return false;
    }
    
    return true;
}

void Server::setupRoutes() {
    // Health check endpoint - PUBLIC
    CROW_ROUTE(app_, "/health")
    ([this]() {
        return handler_->handleHealth();
    });

    // Upload endpoint - PROTECTED
    CROW_ROUTE(app_, "/images").methods(crow::HTTPMethod::POST)
    ([this](const crow::request& req) {
        if (!requireAuth(req)) {
            crow::json::wvalue result;
            result["error"] = "Unauthorized";
            result["message"] = "API key required for write operations";
            return crow::response(401, result);
        }
        return handler_->handleUpload(req);
    });

    // Download endpoint - PUBLIC (read-only)
    CROW_ROUTE(app_, "/images/<string>")
    ([this](const std::string& imageId) {
        return handler_->handleDownload(imageId);
    });

    // Delete endpoint - PROTECTED
    CROW_ROUTE(app_, "/images/<string>").methods(crow::HTTPMethod::DELETE)
    ([this](const crow::request& req, const std::string& imageId) {
        if (!requireAuth(req)) {
            crow::json::wvalue result;
            result["error"] = "Unauthorized";
            result["message"] = "API key required for write operations";
            return crow::response(401, result);
        }
        return handler_->handleDelete(imageId);
    });

    // Named upload endpoint - PROTECTED (root path)
    CROW_ROUTE(app_, "/<string>").methods(crow::HTTPMethod::POST)
    ([this](const crow::request& req, const std::string& imageName) {
        if (!requireAuth(req)) {
            crow::json::wvalue result;
            result["error"] = "Unauthorized";
            result["message"] = "API key required for write operations";
            return crow::response(401, result);
        }
        return handler_->handleNamedUpload(req, imageName);
    });

    // Named download endpoint - PUBLIC (root path)
    CROW_ROUTE(app_, "/<string>")
    ([this](const std::string& imageName) {
        return handler_->handleNamedDownload(imageName);
    });

    // Named delete endpoint - PROTECTED (root path)
    CROW_ROUTE(app_, "/<string>").methods(crow::HTTPMethod::DELETE)
    ([this](const crow::request& req, const std::string& imageName) {
        if (!requireAuth(req)) {
            crow::json::wvalue result;
            result["error"] = "Unauthorized";
            result["message"] = "API key required for write operations";
            return crow::response(401, result);
        }
        return handler_->handleNamedDelete(imageName);
    });
}

void Server::run() {
    std::cout << "Starting image storage server on port " << port_ << std::endl;
    std::cout << "Storage directory: " << storage_->getImagePath("").parent_path() << std::endl;
    std::cout << "\nAvailable endpoints:" << std::endl;
    std::cout << "  POST   /images              - Upload image (returns hash)" << std::endl;
    std::cout << "  GET    /images/<id>         - Download image by hash" << std::endl;
    std::cout << "  DELETE /images/<id>         - Delete image by hash" << std::endl;
    std::cout << "  POST   /<name>.png          - Upload image with name" << std::endl;
    std::cout << "  GET    /<name>.png          - Download image by name" << std::endl;
    std::cout << "  DELETE /<name>.png          - Delete name mapping" << std::endl;
    std::cout << "  GET    /health              - Health check" << std::endl;
    std::cout << std::endl;

    app_.bindaddr("0.0.0.0").port(port_).multithreaded().run();
}

void Server::stop() {
    app_.stop();
}

} // namespace imgstore
