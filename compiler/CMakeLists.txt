cmake_minimum_required(VERSION 2.6)
project(Compiler CXX C)

include_directories(../engine/inc)
include_directories(../engine/inc/gui)
include_directories(../engine/inc/win32)
include_directories(../tinyxml/inc)
include_directories(../glew/inc)
include_directories(inc)

set(COMPILER_SRC
	src/main.cpp
	src/ResourceCompilerImpl.cpp
)

add_executable(compiler ${COMPILER_SRC})
target_link_libraries(compiler replacement-new memory-manager engine tinyxml glew ${COMMONS_LIB})
#set_target_properties(compiler PROPERTIES LINK_FLAGS -shared-libgcc)

add_subdirectory(plugins)