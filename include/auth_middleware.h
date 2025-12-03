#pragma once

#include <string>
#include <optional>

namespace imgstore {

/**
 * @brief Middleware for API key authentication
 */
class AuthMiddleware {
public:
    /**
     * @brief Construct auth middleware with API key
     * @param apiKey The API key to validate against
     */
    explicit AuthMiddleware(const std::string& apiKey);

    /**
     * @brief Validate provided API key
     * @param providedKey The key to validate
     * @return true if valid, false otherwise
     */
    bool validateApiKey(const std::string& providedKey) const;

    /**
     * @brief Extract API key from Authorization header
     * @param authHeader The Authorization header value
     * @return Optional containing the extracted key, or nullopt if invalid
     */
    std::optional<std::string> extractApiKey(const std::string& authHeader) const;

private:
    std::string apiKey_;
};

} // namespace imgstore
