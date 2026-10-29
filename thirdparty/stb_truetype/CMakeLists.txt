cmake_minimum_required(VERSION 3.15...3.30)
project(stb_truetype)

add_library(stb_truetype INTERFACE)

# Change PUBLIC/PRIVATE to INTERFACE for header-only libraries
target_include_directories(stb_truetype INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/include")
