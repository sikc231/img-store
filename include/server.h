#pragma once

#include <memory>
#include <string>
#include "crow_all.h"
#include "storage_manager.h"
#include "image_handler.h"

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
     */
    Server(const std::string& storageDir, int port = 8080);

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
    crow::SimpleApp app_;

    /**
     * @brief Setup HTTP routes
     */
    void setupRoutes();
};

} // namespace imgstore
