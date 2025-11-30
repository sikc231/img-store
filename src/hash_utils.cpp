#include "hash_utils.h"
#include <xxhash.h>
#include <sstream>
#include <iomanip>

namespace imgstore {

uint64_t HashUtils::xxh3_64(const void* data, size_t size) {
    return XXH3_64bits(data, size);
}

uint64_t HashUtils::xxh3_64(const std::string& str) {
    return XXH3_64bits(str.data(), str.size());
}

std::string HashUtils::generateShardPath(uint64_t hash, int depth, int width) {
    std::string hexHash = hashToHex(hash);
    std::string path;
    
    for (int i = 0; i < depth && i * width < hexHash.length(); ++i) {
        if (i > 0) {
            path += "/";
        }
        int start = i * width;
        int len = std::min(width, static_cast<int>(hexHash.length() - start));
        path += hexHash.substr(start, len);
    }
    
    return path;
}

std::string HashUtils::hashToHex(uint64_t hash) {
    std::stringstream ss;
    ss << std::hex << std::setfill('0') << std::setw(16) << hash;
    return ss.str();
}

} // namespace imgstore
