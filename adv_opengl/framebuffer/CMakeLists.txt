cmake_minimum_required( VERSION 3.16.3 )
project( framebuffer )

set ( PROJECT_VERSION_MAJOR 0 )
set ( PROJECT_VERSION_MINOR 1 )
set ( CMAKE_CXX_STANDARD 17 )
set ( CMAKE_BUILD_TYPE Debug )
set ( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_library(glad "${CMAKE_SOURCE_DIR}/include/glad/glad.h" "${CMAKE_SOURCE_DIR}/src/glad.c")

include_directories( include /usr/local/include . )

set ( SRC_FILES main.cpp src/util.cpp )
set( MYAPP "${CMAKE_PROJECT_NAME}_${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}" )


add_definitions( -DDATA_PATH="/home/shinjisoo/openglpractice/DATA" )

add_executable( ${MYAPP} ${SRC_FILES} )
target_link_libraries(${MYAPP} glad glfw3  X11 Xi Xrandr Xcursor rt m pthread dl Xxf86vm Xinerama assimp z )

find_package( OpenGL REQUIRED )
add_compile_options( -g -Wall )


message( STATUS "Compiler Info")
message( STATUS " - Path     \t: ${CMAKE_CXX_COMPILER}")
message( STATUS " - Version  \t: ${CMAKE_CXX_COMPILER_VERSION}")

