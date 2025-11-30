#pragma once

#include <string>
#include <vector>
#include <optional>
#include <filesystem>

namespace imgstore {

/**
 * @brief Manages image storage with sharded filesystem layout
 */
class StorageManager {
public:
    /**
     * @brief Construct a new Storage Manager
     * @param baseDir Base directory for storage
     * @param shardDepth Depth of sharding hierarchy
     */
    explicit StorageManager(const std::string& baseDir, int shardDepth = 3);

    /**
     * @brief Store image data
     * @param imageId Unique identifier for the image
     * @param data Image binary data
     * @return true if successful, false otherwise
     */
    bool storeImage(const std::string& imageId, const std::vector<uint8_t>& data);

    /**
     * @brief Retrieve image data
     * @param imageId Unique identifier for the image
     * @return Optional containing image data if found, nullopt otherwise
     */
    std::optional<std::vector<uint8_t>> retrieveImage(const std::string& imageId);

    /**
     * @brief Delete image
     * @param imageId Unique identifier for the image
     * @return true if successful, false otherwise
     */
    bool deleteImage(const std::string& imageId);

    /**
     * @brief Check if image exists
     * @param imageId Unique identifier for the image
     * @return true if image exists, false otherwise
     */
    bool imageExists(const std::string& imageId);

    /**
     * @brief Get full path for an image
     * @param imageId Unique identifier for the image
     * @return Filesystem path to the image
     */
    std::filesystem::path getImagePath(const std::string& imageId) const;

private:
    std::string baseDir_;
    int shardDepth_;

    /**
     * @brief Ensure directory exists for given path
     * @param path Directory path
     * @return true if directory exists or was created
     */
    bool ensureDirectory(const std::filesystem::path& path);
};

} // namespace imgstore
