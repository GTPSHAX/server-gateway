#pragma once

#include <BaseApp.h>

// Struktur untuk informasi memory
struct MemoryInfo {
    unsigned long long total_phys_mem = 0;    // bytes
    unsigned long long avail_phys_mem = 0;    // bytes
    unsigned long long used_phys_mem = 0;     // bytes
    double usage_percent = 0.0;               // percentage
};

// Struktur untuk informasi CPU
struct CPUInfo {
    double usage_percent = 0.0;               // percentage
};

// Struktur untuk informasi ping
struct PingResult {
    bool success = false;
    double time_ms = -1.0;                    // milliseconds
    int exit_code = -1;
};

// Fungsi utama
class SystemUtils {
public:
    // Memory usage
    static MemoryInfo getMemoryUsage();
    
    // CPU usage
    static CPUInfo getCPUUsage();
    
    // Ping
    static PingResult pingHost(const std::string& host, int timeout_ms = 1000);
    
    // Utility functions
    static void sleepMs(int milliseconds);
    static std::string getOSName();
    static bool isWindows();
    static bool isLinux();
    static bool isMac();
    
private:
    // Helper functions untuk CPU usage (internal)
    static double calculateCPUUsage();
    
#if IS_WINDOWS
    static void initializeWindowsCounters();
    static bool windowsCountersInitialized;
#endif
};