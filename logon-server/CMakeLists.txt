cmake_minimum_required(VERSION 3.10.0)
project(logon-server VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Tambahkan compile options untuk UTF-8
add_compile_options(/utf-8)

file(GLOB UTILS_SRC
  src/utils/*.cpp
)

# Tambahkan executable
add_executable(logon-server
  src/main.cpp
  src/server/ENetServer.cpp
  src/server/ENetServer_static.cpp
  ${UTILS_SRC}
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
target_link_libraries(logon-server enet fmt)