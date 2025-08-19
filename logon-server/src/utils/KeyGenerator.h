#pragma once
#include <string>
#include <chrono>

class KeyGenerator {
public:
    // Generate API key berdasarkan timestamp + random
    static std::string generateAPIKey();
    
    // Generate API key dengan panjang custom
    static std::string generateAPIKey(size_t length);
    
    // Generate API key dengan prefix
    static std::string generateAPIKeyWithPrefix(const std::string& prefix);
    
    // Generate API key berdasarkan user ID
    static std::string generateAPIKeyForUser(int userId);
    
private:
    // Helper functions
    static std::string generateRandomString(size_t length);
    static std::string bytesToHex(const std::string& bytes);
    static unsigned long long getCurrentTimestamp();
};