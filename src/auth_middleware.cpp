#include "auth_middleware.h"
#include <algorithm>

namespace imgstore {

AuthMiddleware::AuthMiddleware(const std::string& apiKey)
    : apiKey_(apiKey) {}

bool AuthMiddleware::validateApiKey(const std::string& providedKey) const {
    // Constant-time comparison to prevent timing attacks
    if (apiKey_.empty() || providedKey.empty()) {
        return false;
    }
    
    if (apiKey_.size() != providedKey.size()) {
        return false;
    }
    
    bool match = true;
    for (size_t i = 0; i < apiKey_.size(); ++i) {
        if (apiKey_[i] != providedKey[i]) {
            match = false;
        }
    }
    
    return match;
}

std::optional<std::string> AuthMiddleware::extractApiKey(const std::string& authHeader) const {
    if (authHeader.empty()) {
        return std::nullopt;
    }
    
    std::string key = authHeader;
    
    // Remove "Bearer " prefix if present
    const std::string bearerPrefix = "Bearer ";
    if (key.size() > bearerPrefix.size() && 
        key.substr(0, bearerPrefix.size()) == bearerPrefix) {
        key = key.substr(bearerPrefix.size());
    }
    
    // Trim whitespace
    key.erase(0, key.find_first_not_of(" \t"));
    key.erase(key.find_last_not_of(" \t") + 1);
    
    return key.empty() ? std::nullopt : std::optional<std::string>(key);
}

} // namespace imgstore
