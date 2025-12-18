#include "image_handler.h"
#include "hash_utils.h"
#include <iostream>

namespace imgstore {

ImageHandler::ImageHandler(std::shared_ptr<StorageManager> storage)
    : storage_(storage) {}

crow::response ImageHandler::handleUpload(const crow::request& req) {
    try {
        // Get image data from request body
        std::vector<uint8_t> imageData(req.body.begin(), req.body.end());

        if (imageData.empty()) {
            return crow::response(400, "Empty image data");
        }

        // Generate unique ID based on content
        std::string imageId = generateImageId(imageData);

        // Check if image already exists
        if (storage_->imageExists(imageId)) {
            crow::json::wvalue result;
            result["id"] = imageId;
            result["status"] = "exists";
            return crow::response(200, result);
        }

        // Store the image
        if (storage_->storeImage(imageId, imageData)) {
            crow::json::wvalue result;
            result["id"] = imageId;
            result["status"] = "uploaded";
            result["size"] = imageData.size();
            return crow::response(201, result);
        } else {
            return crow::response(500, "Failed to store image");
        }
    } catch (const std::exception& e) {
        std::cerr << "Upload error: " << e.what() << std::endl;
        return crow::response(500, "Internal server error");
    }
}

crow::response ImageHandler::handleDownload(const std::string& imageId) {
    try {
        auto imageData = storage_->retrieveImage(imageId);

        if (!imageData) {
            return crow::response(404, "Image not found");
        }

        // Detect content type
        std::string contentType = detectContentType(*imageData);

        // Create response with image data
        crow::response res(200);
        res.set_header("Content-Type", contentType);
        res.body = std::string(imageData->begin(), imageData->end());
        
        return res;
    } catch (const std::exception& e) {
        std::cerr << "Download error: " << e.what() << std::endl;
        return crow::response(500, "Internal server error");
    }
}

crow::response ImageHandler::handleDelete(const std::string& imageId) {
    try {
        if (!storage_->imageExists(imageId)) {
            return crow::response(404, "Image not found");
        }

        if (storage_->deleteImage(imageId)) {
            crow::json::wvalue result;
            result["id"] = imageId;
            result["status"] = "deleted";
            return crow::response(200, result);
        } else {
            return crow::response(500, "Failed to delete image");
        }
    } catch (const std::exception& e) {
        std::cerr << "Delete error: " << e.what() << std::endl;
        return crow::response(500, "Internal server error");
    }
}

crow::response ImageHandler::handleNamedUpload(const crow::request& req, const std::string& imageName) {
    try {
        // Validate image name
        if (imageName.empty()) {
            return crow::response(400, "Image name cannot be empty");
        }

        // Get image data from request body
        std::vector<uint8_t> imageData(req.body.begin(), req.body.end());

        if (imageData.empty()) {
            return crow::response(400, "Empty image data");
        }

        // Generate unique ID based on content
        std::string imageHash = generateImageId(imageData);

        // Check if name already exists
        bool nameExists = storage_->nameMappingExists(imageName);
        auto existingHash = storage_->getHashByName(imageName);

        // Store the image (if not already stored)
        bool imageStored = storage_->imageExists(imageHash);
        if (!imageStored) {
            if (!storage_->storeImage(imageHash, imageData)) {
                return crow::response(500, "Failed to store image");
            }
        }

        // Store or update the name mapping
        if (!storage_->storeNameMapping(imageName, imageHash)) {
            return crow::response(500, "Failed to store name mapping");
        }

        crow::json::wvalue result;
        result["name"] = imageName;
        result["hash"] = imageHash;
        result["size"] = imageData.size();
        
        if (nameExists) {
            result["status"] = "updated";
            result["previous_hash"] = existingHash ? *existingHash : "unknown";
            return crow::response(200, result);
        } else {
            result["status"] = "uploaded";
            return crow::response(201, result);
        }
    } catch (const std::exception& e) {
        std::cerr << "Named upload error: " << e.what() << std::endl;
        return crow::response(500, "Internal server error");
    }
}

crow::response ImageHandler::handleNamedDownload(const std::string& imageName) {
    try {
        // Get hash by name
        auto imageHash = storage_->getHashByName(imageName);
        
        if (!imageHash) {
            return crow::response(404, "Image name not found");
        }

        // Retrieve image data using hash
        auto imageData = storage_->retrieveImage(*imageHash);

        if (!imageData) {
            return crow::response(404, "Image data not found");
        }

        // Detect content type
        std::string contentType = detectContentType(*imageData);

        // Create response with image data
        crow::response res(200);
        res.set_header("Content-Type", contentType);
        res.set_header("X-Image-Hash", *imageHash);
        res.set_header("X-Image-Name", imageName);
        res.body = std::string(imageData->begin(), imageData->end());
        
        return res;
    } catch (const std::exception& e) {
        std::cerr << "Named download error: " << e.what() << std::endl;
        return crow::response(500, "Internal server error");
    }
}

crow::response ImageHandler::handleNamedDelete(const std::string& imageName) {
    try {
        // Get hash by name
        auto imageHash = storage_->getHashByName(imageName);
        
        if (!imageHash) {
            return crow::response(404, "Image name not found");
        }

        // Delete the name mapping
        if (!storage_->deleteNameMapping(imageName)) {
            return crow::response(500, "Failed to delete name mapping");
        }

        // Note: We don't delete the actual image data as it might be referenced by other names
        // or accessed directly by hash

        crow::json::wvalue result;
        result["name"] = imageName;
        result["hash"] = *imageHash;
        result["status"] = "deleted";
        result["note"] = "Name mapping removed. Image data preserved.";
        return crow::response(200, result);
    } catch (const std::exception& e) {
        std::cerr << "Named delete error: " << e.what() << std::endl;
        return crow::response(500, "Internal server error");
    }
}

crow::response ImageHandler::handleHealth() {
    crow::json::wvalue result;
    result["status"] = "healthy";
    result["service"] = "img-store";
    return crow::response(200, result);
}

std::string ImageHandler::generateImageId(const std::vector<uint8_t>& data) {
    uint64_t hash = HashUtils::xxh3_64(data.data(), data.size());
    return HashUtils::hashToHex(hash);
}

std::string ImageHandler::detectContentType(const std::vector<uint8_t>& data) {
    if (data.size() < 4) {
        return "application/octet-stream";
    }

    // Check magic numbers for common image formats
    if (data[0] == 0xFF && data[1] == 0xD8 && data[2] == 0xFF) {
        return "image/jpeg";
    }
    if (data[0] == 0x89 && data[1] == 0x50 && data[2] == 0x4E && data[3] == 0x47) {
        return "image/png";
    }
    if (data[0] == 0x47 && data[1] == 0x49 && data[2] == 0x46) {
        return "image/gif";
    }
    if (data[0] == 0x42 && data[1] == 0x4D) {
        return "image/bmp";
    }
    if (data.size() >= 12 && 
        data[0] == 0x52 && data[1] == 0x49 && data[2] == 0x46 && data[3] == 0x46 &&
        data[8] == 0x57 && data[9] == 0x45 && data[10] == 0x42 && data[11] == 0x50) {
        return "image/webp";
    }

    return "application/octet-stream";
}

} // namespace imgstore
