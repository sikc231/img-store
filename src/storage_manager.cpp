#include "storage_manager.h"
#include "hash_utils.h"
#include <fstream>
#include <iostream>

namespace imgstore {

StorageManager::StorageManager(const std::string& baseDir, int shardDepth)
    : baseDir_(baseDir), shardDepth_(shardDepth) {
    // Ensure base directory exists
    std::filesystem::create_directories(baseDir_);
}

bool StorageManager::storeImage(const std::string& imageId, const std::vector<uint8_t>& data) {
    try {
        auto path = getImagePath(imageId);
        
        // Ensure parent directory exists
        if (!ensureDirectory(path.parent_path())) {
            std::cerr << "Failed to create directory: " << path.parent_path() << std::endl;
            return false;
        }

        // Write image data to file
        std::ofstream file(path, std::ios::binary);
        if (!file) {
            std::cerr << "Failed to open file for writing: " << path << std::endl;
            return false;
        }

        file.write(reinterpret_cast<const char*>(data.data()), data.size());
        file.close();

        return file.good();
    } catch (const std::exception& e) {
        std::cerr << "Error storing image: " << e.what() << std::endl;
        return false;
    }
}

std::optional<std::vector<uint8_t>> StorageManager::retrieveImage(const std::string& imageId) {
    try {
        auto path = getImagePath(imageId);

        if (!std::filesystem::exists(path)) {
            return std::nullopt;
        }

        std::ifstream file(path, std::ios::binary | std::ios::ate);
        if (!file) {
            return std::nullopt;
        }

        auto size = file.tellg();
        file.seekg(0, std::ios::beg);

        std::vector<uint8_t> data(size);
        file.read(reinterpret_cast<char*>(data.data()), size);

        if (!file) {
            return std::nullopt;
        }

        return data;
    } catch (const std::exception& e) {
        std::cerr << "Error retrieving image: " << e.what() << std::endl;
        return std::nullopt;
    }
}

bool StorageManager::deleteImage(const std::string& imageId) {
    try {
        auto path = getImagePath(imageId);

        if (!std::filesystem::exists(path)) {
            return false;
        }

        return std::filesystem::remove(path);
    } catch (const std::exception& e) {
        std::cerr << "Error deleting image: " << e.what() << std::endl;
        return false;
    }
}

bool StorageManager::imageExists(const std::string& imageId) {
    auto path = getImagePath(imageId);
    return std::filesystem::exists(path);
}

std::filesystem::path StorageManager::getImagePath(const std::string& imageId) const {
    // Hash the image ID
    uint64_t hash = HashUtils::xxh3_64(imageId);
    
    // Generate shard path
    std::string shardPath = HashUtils::generateShardPath(hash, shardDepth_, 2);
    
    // Construct full path
    std::filesystem::path fullPath = std::filesystem::path(baseDir_) / shardPath / imageId;
    
    return fullPath;
}

bool StorageManager::ensureDirectory(const std::filesystem::path& path) {
    try {
        return std::filesystem::create_directories(path);
    } catch (const std::exception& e) {
        std::cerr << "Error creating directory: " << e.what() << std::endl;
        return false;
    }
}

} // namespace imgstore
