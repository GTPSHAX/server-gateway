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

#include <enet/enet.h>

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
#elif IS_WINDOWS
  #include <windows.h>
#endif

#if DEBUG
  #include <crtdbg.h>
#endif

#if IS_WINDOWS
  #include <conio.h>
#else
  #include <termios.h>
  #include <unistd.h>
  #include <sys/ioctl.h>
#endif

#include <chrono>

using TimePoint = std::chrono::steady_clock::time_point;
inline auto current_time() {
  return std::chrono::steady_clock::now();
}

