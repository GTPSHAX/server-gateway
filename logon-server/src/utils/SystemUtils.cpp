#include "SystemUtils.h"
#include <iostream>
#include <thread>
#include <chrono>

#if IS_WINDOWS
    #include <windows.h>
    #include <psapi.h>
    #include <pdh.h>
    #pragma comment(lib, "pdh.lib")
    
    bool SystemUtils::windowsCountersInitialized = false;
    static PDH_HQUERY cpuQuery = nullptr;
    static PDH_HCOUNTER cpuCounter = nullptr;
    
#elif IS_LINUX || IS_UNIX
    #include <fstream>
    #include <sstream>
    #include <cstdlib>
    #include <sys/sysinfo.h>
    #include <unistd.h>
    
    #if IS_MAC
        #include <mach/mach.h>
        #include <mach/host_info.h>
        #include <mach/mach_host.h>
    #endif
    
#endif

// Memory usage implementation
MemoryInfo SystemUtils::getMemoryUsage() {
    MemoryInfo mem;
    
#if IS_WINDOWS
    MEMORYSTATUSEX memInfo;
    memInfo.dwLength = sizeof(MEMORYSTATUSEX);
    GlobalMemoryStatusEx(&memInfo);
    
    mem.total_phys_mem = memInfo.ullTotalPhys;
    mem.avail_phys_mem = memInfo.ullAvailPhys;
    mem.used_phys_mem = mem.total_phys_mem - mem.avail_phys_mem;
    mem.usage_percent = (double)mem.used_phys_mem / mem.total_phys_mem * 100.0;
    
#elif IS_LINUX
    std::ifstream file("/proc/meminfo");
    std::string line;
    
    unsigned long long memTotal = 0, memAvailable = 0;
    
    while (std::getline(file, line)) {
        if (line.substr(0, 8) == "MemTotal") {
            memTotal = std::stoull(line.substr(12, line.find_last_of(' ') - 12));
        } else if (line.substr(0, 12) == "MemAvailable") {
            memAvailable = std::stoull(line.substr(16, line.find_last_of(' ') - 16));
        }
    }
    
    mem.total_phys_mem = memTotal * 1024;  // KB to bytes
    mem.avail_phys_mem = memAvailable * 1024;
    mem.used_phys_mem = mem.total_phys_mem - mem.avail_phys_mem;
    mem.usage_percent = (double)mem.used_phys_mem / mem.total_phys_mem * 100.0;
    
#elif IS_MAC
    vm_size_t page_size;
    mach_port_t mach_port = mach_host_self();
    vm_statistics64_data_t vm_stats;
    
    host_page_size(mach_port, &page_size);
    
    mach_msg_type_number_t count = HOST_VM_INFO64_COUNT;
    if (host_statistics64(mach_port, HOST_VM_INFO64, (host_info64_t)&vm_stats, &count) == KERN_SUCCESS) {
        unsigned long long active = vm_stats.active_count * page_size;
        unsigned long long inactive = vm_stats.inactive_count * page_size;
        unsigned long long wired = vm_stats.wire_count * page_size;
        unsigned long long free = vm_stats.free_count * page_size;
        
        mem.used_phys_mem = active + inactive + wired;
        mem.avail_phys_mem = free;
        mem.total_phys_mem = mem.used_phys_mem + mem.avail_phys_mem;
        mem.usage_percent = (double)mem.used_phys_mem / mem.total_phys_mem * 100.0;
    }
    
#endif
    
    return mem;
}

// CPU usage implementation
double SystemUtils::calculateCPUUsage() {
#if IS_WINDOWS
    if (!windowsCountersInitialized) {
        PdhOpenQuery(nullptr, 0, &cpuQuery);
        // FIX: Gunakan string ANSI bukan Unicode
        PdhAddCounter(cpuQuery, "\\Processor(_Total)\\% Processor Time", 0, &cpuCounter);
        PdhCollectQueryData(cpuQuery);
        windowsCountersInitialized = true;
        Sleep(100); // Wait for first sample
    }
    
    PDH_FMT_COUNTERVALUE counterVal;
    PdhCollectQueryData(cpuQuery);
    PdhGetFormattedCounterValue(cpuCounter, PDH_FMT_DOUBLE, nullptr, &counterVal);
    
    return counterVal.doubleValue;
    
#elif IS_LINUX
    static unsigned long long last_idle = 0, last_total = 0;
    
    std::ifstream file("/proc/stat");
    std::string line;
    
    if (std::getline(file, line)) {
        std::istringstream ss(line);
        std::string cpu_label;
        unsigned long long user, nice, system, idle, iowait, irq, softirq, steal;
        
        ss >> cpu_label >> user >> nice >> system >> idle >> iowait >> irq >> softirq >> steal;
        
        unsigned long long current_idle = idle + iowait;
        unsigned long long current_total = user + nice + system + idle + iowait + irq + softirq + steal;
        
        unsigned long long total_diff = current_total - last_total;
        unsigned long long idle_diff = current_idle - last_idle;
        
        double cpu_usage = 100.0 * (total_diff - idle_diff) / total_diff;
        
        last_idle = current_idle;
        last_total = current_total;
        
        return cpu_usage;
    }
    
    return 0.0;
    
#elif IS_MAC
    // Mac implementation - simplified
    host_cpu_load_info_data_t cpuinfo;
    mach_msg_type_number_t count = HOST_CPU_LOAD_INFO_COUNT;
    
    if (host_statistics(mach_host_self(), HOST_CPU_LOAD_INFO, (host_info_t)&cpuinfo, &count) == KERN_SUCCESS) {
        unsigned long long totalTicks = 0;
        for (int i = 0; i < CPU_STATE_MAX; i++) {
            totalTicks += cpuinfo.cpu_ticks[i];
        }
        
        unsigned long long idleTicks = cpuinfo.cpu_ticks[CPU_STATE_IDLE];
        static unsigned long long lastTotalTicks = 0;
        static unsigned long long lastIdleTicks = 0;
        
        unsigned long long totalTicksSinceLast = totalTicks - lastTotalTicks;
        unsigned long long idleTicksSinceLast = idleTicks - lastIdleTicks;
        
        double cpu_usage = 100.0 * (totalTicksSinceLast - idleTicksSinceLast) / totalTicksSinceLast;
        
        lastTotalTicks = totalTicks;
        lastIdleTicks = idleTicks;
        
        return cpu_usage;
    }
    
    return 0.0;
    
#endif
}

CPUInfo SystemUtils::getCPUUsage() {
    CPUInfo cpu;
    cpu.usage_percent = calculateCPUUsage();
    return cpu;
}

// Ping implementation
PingResult SystemUtils::pingHost(const std::string& host, int timeout_ms) {
    PingResult result;
    
#if IS_WINDOWS
    // Gunakan _popen untuk mendapatkan output ping
    std::string command = "ping -n 1 -w " + std::to_string(timeout_ms) + " " + host;
    
    FILE* pipe = _popen(command.c_str(), "r");
    if (!pipe) {
        result.success = false;
        result.exit_code = -1;
        return result;
    }
    
    char buffer[1024];
    std::string output = "";
    
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        output += buffer;
    }
    
    result.exit_code = _pclose(pipe);
    result.success = (result.exit_code == 0);
    
    // Parse waktu ping dari output
    // Contoh: "time<1ms" atau "time=15ms"
    size_t time_pos = output.find("time=");
    if (time_pos != std::string::npos) {
        size_t start = time_pos + 5; // "time=" adalah 5 karakter
        size_t end = output.find("ms", start);
        if (end != std::string::npos) {
            std::string time_str = output.substr(start, end - start);
            try {
                result.time_ms = std::stod(time_str);
            } catch (...) {
                // Jika time<1ms, set ke 0.5ms
                if (output.find("time<1ms") != std::string::npos) {
                    result.time_ms = 0.5;
                }
            }
        }
    } else if (output.find("time<1ms") != std::string::npos) {
        result.time_ms = 0.5; // Kurang dari 1ms
    }
    
#elif IS_LINUX || IS_UNIX
    // Linux/Unix version
    std::string command = "ping -c 1 -W " + std::to_string(timeout_ms/1000) + " " + host;
    
    FILE* pipe = popen(command.c_str(), "r");
    if (!pipe) {
        result.success = false;
        result.exit_code = -1;
        return result;
    }
    
    char buffer[1024];
    std::string output = "";
    
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        output += buffer;
    }
    
    result.exit_code = pclose(pipe);
    result.success = (result.exit_code == 0);
    
    // Parse waktu ping dari output Linux
    // Contoh: "time=15.234 ms"
    size_t time_pos = output.find("time=");
    if (time_pos != std::string::npos) {
        size_t start = time_pos + 5; // "time=" adalah 5 karakter
        size_t end = output.find(" ms", start);
        if (end != std::string::npos) {
            std::string time_str = output.substr(start, end - start);
            try {
                result.time_ms = std::stod(time_str);
            } catch (...) {
                result.time_ms = -1.0;
            }
        }
    }
    
#endif
    
    return result;
}

// Utility functions
void SystemUtils::sleepMs(int milliseconds) {
#if IS_WINDOWS
    Sleep(milliseconds);
#else
    usleep(milliseconds * 1000);
#endif
}

std::string SystemUtils::getOSName() {
#if IS_WINDOWS
    return "Windows";
#elif IS_LINUX
    return "Linux";
#elif IS_MAC
    return "macOS";
#elif IS_UNIX
    return "Unix";
#else
    return "Unknown";
#endif
}

bool SystemUtils::isWindows() {
    return IS_WINDOWS;
}

bool SystemUtils::isLinux() {
    return IS_LINUX;
}

bool SystemUtils::isMac() {
    return IS_MAC;
}

#if IS_WINDOWS
void SystemUtils::initializeWindowsCounters() {
    if (!windowsCountersInitialized) {
        PdhOpenQuery(nullptr, 0, &cpuQuery);
        // FIX: Gunakan string ANSI bukan Unicode
        PdhAddCounter(cpuQuery, "\\Processor(_Total)\\% Processor Time", 0, &cpuCounter);
        PdhCollectQueryData(cpuQuery);
        windowsCountersInitialized = true;
    }
}
#endif