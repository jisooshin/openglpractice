cmake_minimum_required( VERSION 3.16.3 )
project( 
	OpenGLTesting 
	VERSION 0.1 
	PROJECT_DESCRIPTION "예시" 
	LANGUAGE CXX
)


set ( CMAKE_CXX_STANDARD 17 )
set ( PROJECT_VERSION_MAJOR 1 )
set ( PROJECT_VERSION_MINOR 0 )

set ( CMAKE_BUILD_TYPE Debug )
set ( CMAKE_VERBOSE_MAKEFILE true )

set ( 
		SRC_FILES 
			main.cpp
			src/glad.c
			src/util.cpp
	)
add_executable( "${CMAKE_PROJECT_NAME}-${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.out"  $SRC_FILES)

find_package( OpenGL REQUIRED )
find_package( )
set ( CMAKE_C_COMPILER "gcc" )
include_directories( include )
add_compile_options( -g -Wall )
set ( CMAKE_EXE_LINKER_FLAGS "-static -Wl, --gc-sections" )
link_libraries( find_package(GLEW REQUIRED) )
