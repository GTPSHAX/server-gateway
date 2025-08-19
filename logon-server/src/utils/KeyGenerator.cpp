#include "KeyGenerator.h"
#include <random>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <functional>
#include <cstdint>
#include <cstring>


// Fungsi hash sederhana jika tidak menggunakan OpenSSL
inline std::string simpleHash(const std::string& input) {
    std::hash<std::string> hasher;
    size_t hash = hasher(input);
    
    std::stringstream ss;
    ss << std::hex << std::setfill('0');
    
    // Convert hash to hex string
    for (size_t i = 0; i < sizeof(hash); ++i) {
        ss << std::setw(2) << ((hash >> (i * 8)) & 0xFF);
    }
    
    return ss.str();
}

std::string KeyGenerator::generateAPIKey() {
    return generateAPIKey(32); // Default 32 karakter
}

std::string KeyGenerator::generateAPIKey(size_t length) {
    // Dapatkan timestamp
    auto timestamp = getCurrentTimestamp();
    
    // Generate random string
    std::string randomPart = generateRandomString(16);
    
    // Gabungkan timestamp + random
    std::string combined = std::to_string(timestamp) + randomPart;
    
    // Hash untuk membuat key yang lebih acak
    std::string hashed = simpleHash(combined);
    
    // Pastikan panjang sesuai
    if (hashed.length() > length) {
        hashed = hashed.substr(0, length);
    } else if (hashed.length() < length) {
        // Tambahkan random characters jika terlalu pendek
        while (hashed.length() < length) {
            hashed += generateRandomString(length - hashed.length());
        }
    }
    
    // Convert ke uppercase untuk konsistensi
    std::transform(hashed.begin(), hashed.end(), hashed.begin(), ::toupper);
    
    return hashed;
}

std::string KeyGenerator::generateAPIKeyWithPrefix(const std::string& prefix) {
    std::string key = generateAPIKey(32);
    return prefix + "_" + key;
}

std::string KeyGenerator::generateAPIKeyForUser(int userId) {
    auto timestamp = getCurrentTimestamp();
    std::string randomPart = generateRandomString(12);
    std::string combined = std::to_string(userId) + "_" + std::to_string(timestamp) + "_" + randomPart;
    
    std::string hashed = simpleHash(combined);
    std::transform(hashed.begin(), hashed.end(), hashed.begin(), ::toupper);
    
    return hashed.substr(0, 32);
}

// Helper functions
unsigned long long KeyGenerator::getCurrentTimestamp() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()
    ).count();
}

std::string KeyGenerator::generateRandomString(size_t length) {
    static const char charset[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";
    
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> dis(0, sizeof(charset) - 2);
    
    std::string result;
    result.reserve(length);
    
    for (size_t i = 0; i < length; ++i) {
        result += charset[dis(gen)];
    }
    
    return result;
}

std::string KeyGenerator::bytesToHex(const std::string& bytes) {
    std::stringstream hexStream;
    hexStream << std::hex << std::setfill('0');
    
    for (unsigned char byte : bytes) {
        hexStream << std::setw(2) << static_cast<int>(byte);
    }
    
    return hexStream.str();
}