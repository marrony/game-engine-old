cmake_minimum_required(VERSION 2.6)
project(Spikes CXX C)

include_directories("${PROJECT_BINARY_DIR}")

if(DEFINED ENV{CUDA_LIB_PATH})
	link_directories("$ENV{CUDA_LIB_PATH}")
else()
	link_directories("$ENV{AMDAPPSDKROOT}/lib/x86")
endif()

include_directories(../engine/inc)
include_directories(../engine/inc/gui)
include_directories(../engine/inc/win32)
include_directories(../glew/inc)
include_directories(../tinyxml/inc)
include_directories(../game/inc)

set(SPIKES_SRC
	spikes.cpp
)

add_executable(spikes ${SPIKES_SRC})
target_link_libraries(spikes replacement-new memory-manager engine tinyxml glew ${COMMONS_LIB})
