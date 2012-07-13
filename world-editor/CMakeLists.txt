cmake_minimum_required(VERSION 2.6)
project(WorldEditor CXX C)

include_directories(../engine/inc)
include_directories(../engine/inc/gui)
include_directories(../engine/inc/win32)
include_directories(../glew/inc)
include_directories(../tinyxml/inc)
include_directories(inc)

set(EDITOR_SRC
	src/main.cpp
)

add_executable(world-editor ${EDITOR_SRC})
target_link_libraries(world-editor replacement-new memory-manager engine tinyxml glew ${COMMONS_LIB})
