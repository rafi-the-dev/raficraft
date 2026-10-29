cmake_minimum_required(VERSION 3.15...3.30)
project(stb_image)

# Define as an INTERFACE library since it's header-only
add_library(stb_image INTERFACE)

# Expose the correct include directory where stb_image.h actually lives
target_include_directories(stb_image INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/include")
