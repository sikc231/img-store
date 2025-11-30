#pragma once

#include <string>
#include <cstdint>

namespace imgstore {

/**
 * @brief Utility class for hashing operations using xxHash3
 */
class HashUtils {
public:
    /**
     * @brief Compute XXH3 64-bit hash of data
     * @param data Pointer to data buffer
     * @param size Size of data in bytes
     * @return 64-bit hash value
     */
    static uint64_t xxh3_64(const void* data, size_t size);

    /**
     * @brief Compute XXH3 64-bit hash of a string
     * @param str Input string
     * @return 64-bit hash value
     */
    static uint64_t xxh3_64(const std::string& str);

    /**
     * @brief Generate shard path from hash value
     * @param hash Hash value
     * @param depth Depth of sharding (number of directory levels)
     * @param width Characters per directory level
     * @return Shard path (e.g., "a/b/c")
     */
    static std::string generateShardPath(uint64_t hash, int depth = 3, int width = 2);

    /**
     * @brief Convert hash to hex string
     * @param hash Hash value
     * @return Hexadecimal string representation
     */
    static std::string hashToHex(uint64_t hash);
};

} // namespace imgstore
