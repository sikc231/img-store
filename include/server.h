#pragma once

#include <memory>
#include <string>
#include "crow_all.h"
#include "storage_manager.h"
#include "image_handler.h"
#include "auth_middleware.h"

namespace imgstore {

/**
 * @brief Main HTTP server for image storage
 */
class Server {
public:
    /**
     * @brief Construct a new Server
     * @param storageDir Base directory for image storage
     * @param port HTTP server port
     * @param apiKey API key for write operations (empty = no auth)
     */
    Server(const std::string& storageDir, int port = 8080, const std::string& apiKey = "");

    /**
     * @brief Initialize and start the server
     */
    void run();

    /**
     * @brief Stop the server
     */
    void stop();

private:
    int port_;
    std::shared_ptr<StorageManager> storage_;
    std::shared_ptr<ImageHandler> handler_;
    std::shared_ptr<AuthMiddleware> auth_;
    crow::SimpleApp app_;
    bool authEnabled_;

    /**
     * @brief Setup HTTP routes
     */
    void setupRoutes();

    /**
     * @brief Check if request has valid authentication
     * @param req HTTP request
     * @return true if authorized, false otherwise
     */
    bool requireAuth(const crow::request& req);
};

} // namespace imgstore
