cmake_minimum_required(VERSION 3.10.0)
project(logon-server VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Tambahkan compile options untuk UTF-8
add_compile_options(/utf-8 )

file(GLOB UTILS_SRC
  src/utils/*.cpp
)
file(GLOB SERVER_SRC
  src/server/*.cpp
  src/server/**/*.cpp
)
file(GLOB PLAYER_SRC
  src/player/*.cpp
)
file(GLOB PACKET_SRC
  src/packet/*.cpp
)
file(GLOB PROTON_SDK_SRC
  src/SDK/*.cpp
  src/SDK/**/*.cpp
  src/SDK/**/**/*.cpp
)

# Tambahkan executable
add_executable(logon-server
  src/main.cpp
  ${SERVER_SRC}
  ${PACKET_SRC}
  ${PLAYER_SRC}
  ${UTILS_SRC}
  ${PROTON_SDK_SRC}
)

# Terapkan compile options ke target specific
target_compile_options(logon-server PRIVATE /utf-8)

# Tambahkan include directories
target_include_directories(logon-server PRIVATE
  src/
  src/libs/enet/include
  src/libs/fmt/include
  src/libs/nlohmann-json/include
)

add_subdirectory(src/libs/enet)
add_subdirectory(src/libs/fmt)

# Link libraries
find_package(CURL REQUIRED)
target_link_libraries(logon-server
  enet
  fmt
  CURL::libcurl
  ws2_32
  winmm
)