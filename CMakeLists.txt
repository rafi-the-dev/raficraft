#https://github.com/meemknight/cmakeSetup

cmake_minimum_required(VERSION 3.15...3.30)

set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>") #we want to use the static runtime

project(mygame)

option(RAFICRAFT_PORTABLE "Use bundled resources for distribution" OFF)

if(RAFICRAFT_PORTABLE)
    include(CheckIPOSupported)
    check_ipo_supported(RESULT ipoSupported OUTPUT ipoError)
    if(NOT ipoSupported)
        message(FATAL_ERROR "Release link-time optimization unavailable: ${ipoError}")
    endif()
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE ON)
endif()


set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
add_subdirectory(thirdparty/glfw-3.3.2)			#window oppener
add_subdirectory(thirdparty/glad)				#opengl loader
add_subdirectory(thirdparty/stb_image)			#loading immaged
add_subdirectory(thirdparty/stb_truetype)		#loading ttf files
add_subdirectory(thirdparty/glm)				#math
add_subdirectory(thirdparty/gl2d)
add_subdirectory(thirdparty/glui)
add_subdirectory(thirdparty/imgui-docking)		#ui
add_subdirectory(thirdparty/FastNoiseSIMD)
# The SIMD backends define different vector types with the same internal names.
set_property(TARGET FastNoiseSIMD PROPERTY INTERPROCEDURAL_OPTIMIZATION_RELEASE OFF)


# Define MY_SOURCES to be a list of all the source files for my game 
file(GLOB_RECURSE MY_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")


add_executable("${CMAKE_PROJECT_NAME}")

set_property(TARGET "${CMAKE_PROJECT_NAME}" PROPERTY CXX_STANDARD 17)

if(RAFICRAFT_PORTABLE)
    target_compile_definitions("${CMAKE_PROJECT_NAME}" PRIVATE RESOURCES_PATH="./resources/")
    if(WIN32)
        set_property(TARGET "${CMAKE_PROJECT_NAME}" PROPERTY WIN32_EXECUTABLE ON)
        if(MSVC)
            target_link_options("${CMAKE_PROJECT_NAME}" PRIVATE /ENTRY:mainCRTStartup)
        endif()
    endif()
else()
    target_compile_definitions("${CMAKE_PROJECT_NAME}" PRIVATE RESOURCES_PATH="${CMAKE_CURRENT_SOURCE_DIR}/resources/")
endif()

target_sources("${CMAKE_PROJECT_NAME}" PRIVATE ${MY_SOURCES} )


if(MSVC) # If using the VS compiler...

	target_compile_definitions("${CMAKE_PROJECT_NAME}" PUBLIC _CRT_SECURE_NO_WARNINGS)

	#remove console
	#set_target_properties("${CMAKE_PROJECT_NAME}" PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
	
	set_property(TARGET "${CMAKE_PROJECT_NAME}" PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

endif()


target_include_directories("${CMAKE_PROJECT_NAME}" PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include/")


target_link_libraries("${CMAKE_PROJECT_NAME}" PRIVATE glm glfw 
	glad stb_image stb_truetype imgui FastNoiseSIMD glui)


if(MINGW)
    target_link_options("${CMAKE_PROJECT_NAME}" PRIVATE -static -static-libgcc -static-libstdc++)
endif()

if(RAFICRAFT_PORTABLE)
    install(TARGETS "${CMAKE_PROJECT_NAME}" RUNTIME DESTINATION .)
    install(DIRECTORY resources DESTINATION .)
    install(FILES README.md DESTINATION .)
    install(FILES thirdparty/FastNoiseSIMD/LICENSE DESTINATION licenses RENAME FastNoiseSIMD.txt)
    install(FILES thirdparty/glfw-3.3.2/LICENSE.md DESTINATION licenses RENAME GLFW.txt)
    if(WIN32)
        install(FILES packaging/Play.bat DESTINATION .)
    else()
        install(PROGRAMS packaging/play.sh DESTINATION .)
        install(PROGRAMS packaging/Play.desktop DESTINATION .)
    endif()
endif()
