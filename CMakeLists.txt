cmake_minimum_required(VERSION 2.6)
project(Game CXX C)

add_definitions(-D__GXX_EXPERIMENTAL_CXX0X__)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++0x")
	
if(CMAKE_BUILD_TYPE STREQUAL "debug")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage -pg")
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fprofile-arcs -ftest-coverage -pg")
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fprofile-arcs -ftest-coverage -pg")
	set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -fprofile-arcs -ftest-coverage -pg")
	set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} -fprofile-arcs -ftest-coverage -pg")
endif()

SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)

#add_definitions(-D__SSE__)
#add_definitions(-DTIXML_USE_STL)

# The version number.
#set(Tutorial_VERSION_MAJOR 1)
#set(Tutorial_VERSION_MINOR 0)

# configure a header file to pass some of the CMake settings
# to the source code
#configure_file(
#  "${PROJECT_SOURCE_DIR}/TutorialConfig.h.in"
#  "${PROJECT_BINARY_DIR}/TutorialConfig.h"
#)
 
# add the binary tree to the search path for include files
# so that we will find TutorialConfig.h
include_directories("${PROJECT_BINARY_DIR}")

if(DEFINED ENV{CUDA_LIB_PATH})
	link_directories("$ENV{CUDA_LIB_PATH}")
else()
	link_directories("$ENV{AMDAPPSDKROOT}/lib/x86")
endif()

if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
	set(COMMONS_LIB opengl32 gdi32 user32 winmm)
endif()

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	set(COMMONS_LIB GL pthread)
endif()


add_subdirectory(engine)
add_subdirectory(game)
#add_subdirectory(world-editor)
#add_subdirectory(character-editor)
add_subdirectory(glew)
add_subdirectory(gmock)
add_subdirectory(gtest)
add_subdirectory(tinyxml)
add_subdirectory(compiler)
add_subdirectory(spikes)

include_directories(engine/inc)
include_directories(engine/inc/gui)
include_directories(engine/inc/win32)
include_directories(glew/inc)
include_directories(tinyxml/inc)
include_directories(game/inc)
