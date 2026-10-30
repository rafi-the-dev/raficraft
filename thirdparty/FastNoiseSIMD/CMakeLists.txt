add_library(FastNoiseSIMD STATIC
    FastNoiseSIMD/FastNoiseSIMD.cpp
    FastNoiseSIMD/FastNoiseSIMD_sse2.cpp
    FastNoiseSIMD/FastNoiseSIMD_sse41.cpp
    FastNoiseSIMD/FastNoiseSIMD_avx2.cpp
    FastNoiseSIMD/FastNoiseSIMD_avx512.cpp
    FastNoiseSIMD/FastNoiseSIMD_neon.cpp
)

target_include_directories(FastNoiseSIMD PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/FastNoiseSIMD")
target_compile_features(FastNoiseSIMD PRIVATE cxx_std_11)

# Each backend is compiled separately so runtime CPU detection stays portable.
# FastNoiseSIMD_internal.cpp is included by the backends, not compiled directly.
if(CMAKE_SYSTEM_PROCESSOR MATCHES "^(x86_64|AMD64|amd64|i[3-6]86|x86)$")
    if(MSVC)
        set_source_files_properties(FastNoiseSIMD/FastNoiseSIMD_avx2.cpp
            PROPERTIES COMPILE_OPTIONS "/arch:AVX2;/GL-")
    else()
        set_source_files_properties(FastNoiseSIMD/FastNoiseSIMD_sse2.cpp
            PROPERTIES COMPILE_OPTIONS "-msse2")
        set_source_files_properties(FastNoiseSIMD/FastNoiseSIMD_sse41.cpp
            PROPERTIES COMPILE_OPTIONS "-msse4.1")
        set_source_files_properties(FastNoiseSIMD/FastNoiseSIMD_avx2.cpp
            PROPERTIES COMPILE_OPTIONS "-mavx2;-mfma")
    endif()
endif()
