#pragma once

// Platform detection
#if defined(WIN32) || defined(_WIN32)
  #define IS_WIN32 1
#else
  #define IS_WIN32 0
#endif

#if defined(WIN64) || defined(_WIN64)
  #define IS_WIN64 1
#else
  #define IS_WIN64 0
#endif

#if IS_WIN32 || IS_WIN64
  #define IS_WINDOWS 1
#else
  #define IS_WINDOWS 0
#endif

#if defined(unix) || defined(__unix) || defined(__unix__)
  #define IS_UNIX 1
#else
  #define IS_UNIX 0
#endif

#if defined(linux) || defined(__linux) || defined(__linux__)
  #define IS_LINUX 1
#else
  #define IS_LINUX 0
#endif

#if defined(__ANDROID__)
  #define IS_ANDROID 1
#else
  #define IS_ANDROID 0
#endif

#if defined(macintosh) || defined(__APPLE__) || defined(__MACH__)
  #define IS_MAC 1
#else
  #define IS_MAC 0
#endif

// Debug detection
#if defined(DEBUG) || defined(_DEBUG)
  #define IS_DEBUG 1
#else
  #define IS_DEBUG 0
#endif

#if defined(NDEBUG) || defined(_NDEBUG)
  #define IS_NDEBUG 1
#else
  #define IS_NDEBUG 0
#endif

#include <io.h>
#include <string>
#include <exception>
#include <algorithm>
#include <array>
#include <cctype>
#include <chrono>
#include <climits>
#include <condition_variable> // std::condition_variable for threading synchronization
#include <cstdint>            // Fixed-width integer types (uint8_t, int32_t, etc.)
#include <cstdio>             // C-style I/O (e.g., printf, scanf, FILE operations)
#include <cstdlib>            // General utilities (e.g., abs, rand, malloc, free)
#include <csignal>            // Signal handling (e.g., SIGINT, SIGTERM)
#include <cstring>            // C-style string manipulation (e.g., strlen, strcpy, memset, memcpy)
#include <ctime>              // C-style time utilities (e.g., time, ctime)
#include <filesystem>         // C++17 filesystem library (std::filesystem)
#include <format>             // C++20 formatting library (std::format)
#include <fstream>            // File stream operations (ifstream, ofstream, fstream)
#include <functional>         // Function objects, std::function, std::bind
#include <future>             // Asynchronous operations (std::future, std::async, std::promise)
#include <iomanip>            // Input/output manipulators (e.g., std::setprecision, std::setw)
#include <iostream>           // Standard I/O (std::cin, std::cout, std::cerr)
#include <map>                // std::map associative container
#include <memory>             // Smart pointers (std::unique_ptr, std::shared_ptr, std::weak_ptr)
#include <mutex>              // std::mutex for threading synchronization
#include <numeric>            // Numeric operations (e.g., std::accumulate, std::iota)
#include <queue>              // std::queue container adaptor
#include <random>             // Random number generation engines and distributions
#include <ranges>             // C++20 ranges library
#include <regex>              // Regular expressions (std::regex)
#include <sstream>            // String stream operations (std::stringstream)
#include <string_view>        // C++17 std::string_view
#include <type_traits>        // Compile-time type information (e.g., is_same, enable_if)
#include <unordered_map>      // std::unordered_map hash table
#include <unordered_set>      // std::unordered_set hash set
#include <variant>            // C++17 std::variant (sum type)
#include <vector>             // std::vector dynamic array
#include <thread>
#include <limits>   // For std::numeric_limits
#include <math.h>             // C-style math functions (for C++, prefer <cmath>)
#include <chrono>

#if IS_LINUX || IS_UNIX
#include <cstring>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#define Sleep(x) usleep((x) * 1000)
#define BYTE unsigned char  // Common Windows types equivalents
#define DWORD unsigned int  // Common Windows types equivalents
#define __int64 int64_t     // Standard C++11 fixed-width integers
#define __int32 int32_t     // Standard C++11 fixed-width integers
#define __int16 int16_t     // Standard C++11 fixed-width integers
#define __int8 int8_t       // Standard C++11 fixed-width integers
#endif

#if DEBUG
  #include <crtdbg.h>
#endif

#include <enet/enet.h>
#include <fmt/format.h>
#include <fmt/color.h>
#include <nlohmann/json.hpp>

#if IS_WINDOWS
#include <conio.h>
#else
#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>
#endif

using TimePoint = std::chrono::steady_clock::time_point;
inline auto current_time() {
  return std::chrono::steady_clock::now();
}

