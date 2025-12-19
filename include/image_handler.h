#pragma once

#include <memory>
#include "crow_all.h"
#include "storage_manager.h"

namespace imgstore {

/**
 * @brief Handles HTTP requests for image operations
 */
class ImageHandler {
public:
    /**
     * @brief Construct a new Image Handler
     * @param storage Shared pointer to storage manager
     */
    explicit ImageHandler(std::shared_ptr<StorageManager> storage);

    /**
     * @brief Handle image upload request
     * @param req HTTP request
     * @return HTTP response
     */
    crow::response handleUpload(const crow::request& req);

    /**
     * @brief Handle image download request
     * @param imageId Unique identifier for the image
     * @return HTTP response
     */
    crow::response handleDownload(const std::string& imageId);

    /**
     * @brief Handle image delete request
     * @param imageId Unique identifier for the image
     * @return HTTP response
     */
    crow::response handleDelete(const std::string& imageId);

    /**
     * @brief Handle named image upload request
     * @param req HTTP request
     * @param imageName User-friendly name for the image
     * @return HTTP response
     */
    crow::response handleNamedUpload(const crow::request& req, const std::string& imageName);

    /**
     * @brief Handle named image download request
     * @param imageName User-friendly name for the image
     * @return HTTP response
     */
    crow::response handleNamedDownload(const std::string& imageName);

    /**
     * @brief Handle named image delete request
     * @param imageName User-friendly name for the image
     * @return HTTP response
     */
    crow::response handleNamedDelete(const std::string& imageName);

    /**
     * @brief Handle health check request
     * @return HTTP response
     */
    crow::response handleHealth();

    /**
     * @brief Handle list all names request
     * @return HTTP response with array of image names
     */
    crow::response handleListNames();

private:
    std::shared_ptr<StorageManager> storage_;

    /**
     * @brief Generate unique image ID from content
     * @param data Image data
     * @return Unique image identifier
     */
    std::string generateImageId(const std::vector<uint8_t>& data);

    /**
     * @brief Detect content type from image data
     * @param data Image data
     * @return MIME type string
     */
    std::string detectContentType(const std::vector<uint8_t>& data);
};

} // namespace imgstore
