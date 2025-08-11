#pragma once

#include <unordered_map>
#include <string>
#include <vector>
#include <memory>
#include <mutex>
#include <chrono>
#include <variant>
#include <bitset>

/**
 * @fileoverview CacheManager - Lightweight memory cache system with bit-efficient storage
 *
 * @example Basic Usage:
 * ```cpp
 * // Store different types of data
 * CacheManager::set("user_id", 12345);
 * CacheManager::set("username", "john_doe");
 * CacheManager::set("is_active", true);
 * CacheManager::set("score", 98.5f);
 *
 * // Retrieve data
 * int userId = CacheManager::getInt("user_id");
 * std::string username = CacheManager::getString("username");
 * bool active = CacheManager::getBool("is_active");
 * float score = CacheManager::getFloat("score");
 *
 * // Check if key exists
 * if (CacheManager::exists("user_id")) {
 *     // Key exists
 * }
 *
 * // Remove specific key
 * CacheManager::remove("old_data");
 *
 * // Clear all cache
 * CacheManager::clear();
 * ```
 *
 * @example Advanced Usage with TTL:
 * ```cpp
 * // Set data with 60 seconds TTL
 * CacheManager::setWithTTL("session_token", "abc123", 60);
 *
 * // Check if still valid
 * if (CacheManager::isValid("session_token")) {
 *     std::string token = CacheManager::getString("session_token");
 * }
 *
 * // Get all keys
 * std::vector<std::string> keys = CacheManager::getKeys();
 *
 * // Get cache statistics
 * auto stats = CacheManager::getStats();
 * std::cout << "Cache size: " << stats.size << std::endl;
 * std::cout << "Memory usage: " << stats.memoryUsage << " bytes" << std::endl;
 * ```
 *
 * @example Bit Operations:
 * ```cpp
 * // Store flags as bits (memory efficient)
 * std::bitset<8> permissions;
 * permissions.set(0); // read permission
 * permissions.set(2); // write permission
 * CacheManager::setBits("user_permissions", permissions);
 *
 * // Retrieve and check permissions
 * auto perms = CacheManager::getBits("user_permissions");
 * if (perms.test(0)) {
 *     // User has read permission
 * }
 * ```
 */

class CacheManager {
public:
    // Data types supported by cache
    using CacheValue = std::variant<
        int,
        long long,
        float,
        double,
        bool,
        std::string,
        std::vector<uint8_t>,
        std::bitset<64>
    >;

    // Cache entry with optional TTL
    struct CacheEntry {
        CacheValue value;
        std::chrono::steady_clock::time_point createdAt;
        std::chrono::seconds ttl;
        bool hasTTL;

        CacheEntry(const CacheValue& val)
            : value(val), createdAt(std::chrono::steady_clock::now()),
            ttl(0), hasTTL(false) {
        }

        CacheEntry(const CacheValue& val, std::chrono::seconds ttlSeconds)
            : value(val), createdAt(std::chrono::steady_clock::now()),
            ttl(ttlSeconds), hasTTL(true) {
        }
    };

    // Cache statistics
    struct CacheStats {
        size_t size;
        size_t memoryUsage;
        size_t hits;
        size_t misses;
        double hitRate;
    };

    // CRUD Operations

    /**
     * @brief Store integer value in cache
     * @param key The cache key
     * @param value Integer value to store
     */
    static void set(const std::string& key, int value);

    /**
     * @brief Store long long value in cache
     * @param key The cache key
     * @param value Long long value to store
     */
    static void set(const std::string& key, long long value);

    /**
     * @brief Store float value in cache
     * @param key The cache key
     * @param value Float value to store
     */
    static void set(const std::string& key, float value);

    /**
     * @brief Store double value in cache
     * @param key The cache key
     * @param value Double value to store
     */
    static void set(const std::string& key, double value);

    /**
     * @brief Store boolean value in cache
     * @param key The cache key
     * @param value Boolean value to store
     */
    static void set(const std::string& key, bool value);

    /**
     * @brief Store string value in cache
     * @param key The cache key
     * @param value String value to store
     */
    static void set(const std::string& key, const std::string& value);

    /**
     * @brief Store binary data in cache
     * @param key The cache key
     * @param value Vector of bytes to store
     */
    static void set(const std::string& key, const std::vector<uint8_t>& value);

    /**
     * @brief Store bitset in cache (memory efficient for flags)
     * @param key The cache key
     * @param value Bitset to store
     */
    static void setBits(const std::string& key, const std::bitset<64>& value);

    /**
     * @brief Store value with TTL (Time To Live)
     * @param key The cache key
     * @param value Value to store
     * @param ttlSeconds TTL in seconds
     */
    template<typename T>
    static void setWithTTL(const std::string& key, const T& value, int ttlSeconds);

    /**
     * @brief Get integer value from cache
     * @param key The cache key
     * @param defaultValue Default value if key not found
     * @return Integer value or default
     */
    static int getInt(const std::string& key, int defaultValue = 0);

    /**
     * @brief Get long long value from cache
     * @param key The cache key
     * @param defaultValue Default value if key not found
     * @return Long long value or default
     */
    static long long getLongLong(const std::string& key, long long defaultValue = 0);

    /**
     * @brief Get float value from cache
     * @param key The cache key
     * @param defaultValue Default value if key not found
     * @return Float value or default
     */
    static float getFloat(const std::string& key, float defaultValue = 0.0f);

    /**
     * @brief Get double value from cache
     * @param key The cache key
     * @param defaultValue Default value if key not found
     * @return Double value or default
     */
    static double getDouble(const std::string& key, double defaultValue = 0.0);

    /**
     * @brief Get boolean value from cache
     * @param key The cache key
     * @param defaultValue Default value if key not found
     * @return Boolean value or default
     */
    static bool getBool(const std::string& key, bool defaultValue = false);

    /**
     * @brief Get string value from cache
     * @param key The cache key
     * @param defaultValue Default value if key not found
     * @return String value or default
     */
    static std::string getString(const std::string& key, const std::string& defaultValue = "");

    /**
     * @brief Get binary data from cache
     * @param key The cache key
     * @return Vector of bytes or empty vector if not found
     */
    static std::vector<uint8_t> getBytes(const std::string& key);

    /**
     * @brief Get bitset from cache
     * @param key The cache key
     * @return Bitset or empty bitset if not found
     */
    static std::bitset<64> getBits(const std::string& key);

    /**
     * @brief Check if key exists and is valid (not expired)
     * @param key The cache key
     * @return True if exists and valid
     */
    static bool exists(const std::string& key);

    /**
     * @brief Check if key exists and is still valid (TTL check)
     * @param key The cache key
     * @return True if exists and not expired
     */
    static bool isValid(const std::string& key);

    /**
     * @brief Remove specific key from cache
     * @param key The cache key to remove
     * @return True if key was removed
     */
    static bool remove(const std::string& key);

    /**
     * @brief Clear all cache entries
     */
    static void clear();

    /**
     * @brief Get all cache keys
     * @return Vector of all keys in cache
     */
    static std::vector<std::string> getKeys();

    /**
     * @brief Get cache statistics
     * @return CacheStats structure with cache information
     */
    static CacheStats getStats();

    /**
     * @brief Clean up expired entries
     * @return Number of entries removed
     */
    static size_t cleanupExpired();

    /**
     * @brief Get memory usage estimation in bytes
     * @return Estimated memory usage
     */
    static size_t getMemoryUsage();

private:
    static std::unordered_map<std::string, std::unique_ptr<CacheEntry>> cache_;
    static std::mutex mutex_;
    static size_t hits_;
    static size_t misses_;

    // Internal helper methods
    static bool isExpired(const CacheEntry* entry);
    static size_t estimateEntrySize(const std::string& key, const CacheEntry* entry);

    // Template implementation for setWithTTL
    template<typename T>
    static void setWithTTLImpl(const std::string& key, const T& value, int ttlSeconds);
};

// Template method implementation
template<typename T>
void CacheManager::setWithTTL(const std::string& key, const T& value, int ttlSeconds) {
    setWithTTLImpl(key, value, ttlSeconds);
}
