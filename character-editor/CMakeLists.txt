cmake_minimum_required(VERSION 2.6)
project(CharEditor CXX C)

include_directories(../engine/inc)
include_directories(../engine/inc/gui)
include_directories(../engine/inc/win32)
include_directories(../glew/inc)
include_directories(../tinyxml/inc)
include_directories(inc)

set(CHARACTER_EDITOR_SRC
	src/main/main.cpp
)

add_executable(character-editor ${CHARACTER_EDITOR_SRC})
target_link_libraries(character-editor engine tinyxml glew ${COMMONS_LIB})
