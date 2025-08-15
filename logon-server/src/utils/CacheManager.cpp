#include "CacheManager.h"

// Static member definitions
std::unordered_map<std::string, std::unique_ptr<CacheManager::CacheEntry>> CacheManager::cache_;
std::mutex CacheManager::mutex_;
size_t CacheManager::hits_ = 0;
size_t CacheManager::misses_ = 0;

// CREATE operations
void CacheManager::set(const std::string& key, int value) {
    std::lock_guard<std::mutex> lock(mutex_);
    cache_[key] = std::make_unique<CacheEntry>(CacheValue(value));
}

void CacheManager::set(const std::string& key, long long value) {
    std::lock_guard<std::mutex> lock(mutex_);
    cache_[key] = std::make_unique<CacheEntry>(CacheValue(value));
}

void CacheManager::set(const std::string& key, float value) {
    std::lock_guard<std::mutex> lock(mutex_);
    cache_[key] = std::make_unique<CacheEntry>(CacheValue(value));
}

void CacheManager::set(const std::string& key, double value) {
    std::lock_guard<std::mutex> lock(mutex_);
    cache_[key] = std::make_unique<CacheEntry>(CacheValue(value));
}

void CacheManager::set(const std::string& key, bool value) {
    std::lock_guard<std::mutex> lock(mutex_);
    cache_[key] = std::make_unique<CacheEntry>(CacheValue(value));
}

void CacheManager::set(const std::string& key, const std::string& value) {
    std::lock_guard<std::mutex> lock(mutex_);
    cache_[key] = std::make_unique<CacheEntry>(CacheValue(value));
}

void CacheManager::set(const std::string& key, const std::vector<uint8_t>& value) {
    std::lock_guard<std::mutex> lock(mutex_);
    cache_[key] = std::make_unique<CacheEntry>(CacheValue(value));
}

void CacheManager::setBits(const std::string& key, const std::bitset<64>& value) {
    std::lock_guard<std::mutex> lock(mutex_);
    cache_[key] = std::make_unique<CacheEntry>(CacheValue(value));
}

// Template implementation for TTL
template<typename T>
void CacheManager::setWithTTLImpl(const std::string& key, const T& value, int ttlSeconds) {
    std::lock_guard<std::mutex> lock(mutex_);
    cache_[key] = std::make_unique<CacheEntry>(
        CacheValue(value),
        std::chrono::seconds(ttlSeconds)
    );
}

// Explicit template instantiations
template void CacheManager::setWithTTLImpl<int>(const std::string&, const int&, int);
template void CacheManager::setWithTTLImpl<long long>(const std::string&, const long long&, int);
template void CacheManager::setWithTTLImpl<float>(const std::string&, const float&, int);
template void CacheManager::setWithTTLImpl<double>(const std::string&, const double&, int);
template void CacheManager::setWithTTLImpl<bool>(const std::string&, const bool&, int);
template void CacheManager::setWithTTLImpl<std::string>(const std::string&, const std::string&, int);
template void CacheManager::setWithTTLImpl<std::vector<uint8_t>>(const std::string&, const std::vector<uint8_t>&, int);
template void CacheManager::setWithTTLImpl<std::bitset<64>>(const std::string&, const std::bitset<64>&, int);

// READ operations
int CacheManager::getInt(const std::string& key, int defaultValue) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = cache_.find(key);
    if (it != cache_.end() && !isExpired(it->second.get())) {
        hits_++;
        try {
            return std::get<int>(it->second->value);
        }
        catch (const std::bad_variant_access&) {
            // Type mismatch, return default
            return defaultValue;
        }
    }
    misses_++;
    return defaultValue;
}

long long CacheManager::getLongLong(const std::string& key, long long defaultValue) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = cache_.find(key);
    if (it != cache_.end() && !isExpired(it->second.get())) {
        hits_++;
        try {
            return std::get<long long>(it->second->value);
        }
        catch (const std::bad_variant_access&) {
            return defaultValue;
        }
    }
    misses_++;
    return defaultValue;
}

float CacheManager::getFloat(const std::string& key, float defaultValue) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = cache_.find(key);
    if (it != cache_.end() && !isExpired(it->second.get())) {
        hits_++;
        try {
            return std::get<float>(it->second->value);
        }
        catch (const std::bad_variant_access&) {
            return defaultValue;
        }
    }
    misses_++;
    return defaultValue;
}

double CacheManager::getDouble(const std::string& key, double defaultValue) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = cache_.find(key);
    if (it != cache_.end() && !isExpired(it->second.get())) {
        hits_++;
        try {
            return std::get<double>(it->second->value);
        }
        catch (const std::bad_variant_access&) {
            return defaultValue;
        }
    }
    misses_++;
    return defaultValue;
}

bool CacheManager::getBool(const std::string& key, bool defaultValue) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = cache_.find(key);
    if (it != cache_.end() && !isExpired(it->second.get())) {
        hits_++;
        try {
            return std::get<bool>(it->second->value);
        }
        catch (const std::bad_variant_access&) {
            return defaultValue;
        }
    }
    misses_++;
    return defaultValue;
}

std::string CacheManager::getString(const std::string& key, const std::string& defaultValue) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = cache_.find(key);
    if (it != cache_.end() && !isExpired(it->second.get())) {
        hits_++;
        try {
            return std::get<std::string>(it->second->value);
        }
        catch (const std::bad_variant_access&) {
            return defaultValue;
        }
    }
    misses_++;
    return defaultValue;
}

std::vector<uint8_t> CacheManager::getBytes(const std::string& key) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = cache_.find(key);
    if (it != cache_.end() && !isExpired(it->second.get())) {
        hits_++;
        try {
            return std::get<std::vector<uint8_t>>(it->second->value);
        }
        catch (const std::bad_variant_access&) {
            return std::vector<uint8_t>();
        }
    }
    misses_++;
    return std::vector<uint8_t>();
}

std::bitset<64> CacheManager::getBits(const std::string& key) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = cache_.find(key);
    if (it != cache_.end() && !isExpired(it->second.get())) {
        hits_++;
        try {
            return std::get<std::bitset<64>>(it->second->value);
        }
        catch (const std::bad_variant_access&) {
            return std::bitset<64>();
        }
    }
    misses_++;
    return std::bitset<64>();
}

// Utility operations
bool CacheManager::exists(const std::string& key) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = cache_.find(key);
    return it != cache_.end() && !isExpired(it->second.get());
}

bool CacheManager::isValid(const std::string& key) {
    return exists(key); // Same implementation for now
}

// DELETE operations
bool CacheManager::remove(const std::string& key) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = cache_.find(key);
    if (it != cache_.end()) {
        cache_.erase(it);
        return true;
    }
    return false;
}

void CacheManager::clear() {
    std::lock_guard<std::mutex> lock(mutex_);
    cache_.clear();
    hits_ = 0;
    misses_ = 0;
}

// Utility methods
std::vector<std::string> CacheManager::getKeys() {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<std::string> keys;
    keys.reserve(cache_.size());

    for (const auto& pair : cache_) {
        if (!isExpired(pair.second.get())) {
            keys.push_back(pair.first);
        }
    }
    return keys;
}

CacheManager::CacheStats CacheManager::getStats() {
    std::lock_guard<std::mutex> lock(mutex_);

    size_t validEntries = 0;
    size_t memoryUsage = 0;

    for (const auto& pair : cache_) {
        if (!isExpired(pair.second.get())) {
            validEntries++;
            memoryUsage += estimateEntrySize(pair.first, pair.second.get());
        }
    }

    size_t totalAccess = hits_ + misses_;
    double hitRate = totalAccess > 0 ? static_cast<double>(hits_) / totalAccess : 0.0;

    return CacheStats{
        validEntries,
        memoryUsage,
        hits_,
        misses_,
        hitRate
    };
}

size_t CacheManager::cleanupExpired() {
    std::lock_guard<std::mutex> lock(mutex_);
    size_t removedCount = 0;

    auto it = cache_.begin();
    while (it != cache_.end()) {
        if (isExpired(it->second.get())) {
            it = cache_.erase(it);
            removedCount++;
        }
        else {
            ++it;
        }
    }

    return removedCount;
}

size_t CacheManager::getMemoryUsage() {
    return getStats().memoryUsage;
}

// Private helper methods
bool CacheManager::isExpired(const CacheEntry* entry) {
    if (!entry->hasTTL) {
        return false; // No TTL set, never expires
    }

    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - entry->createdAt);
    return elapsed >= entry->ttl;
}

size_t CacheManager::estimateEntrySize(const std::string& key, const CacheEntry* entry) {
    size_t size = key.size() + sizeof(CacheEntry);

    std::visit([&size](const auto& value) {
        using T = std::decay_t<decltype(value)>;
        if constexpr (std::is_same_v<T, std::string>) {
            size += value.size();
        }
        else if constexpr (std::is_same_v<T, std::vector<uint8_t>>) {
            size += value.size();
        }
        else {
            size += sizeof(value);
        }
        }, entry->value);

    return size;
}
