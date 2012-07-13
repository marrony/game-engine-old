cmake_minimum_required(VERSION 2.6)
project(DemoGame CXX C)

include_directories(../engine/inc)
include_directories(../engine/inc/gui)
include_directories(../engine/inc/win32)
include_directories(../glew/inc)
include_directories(../tinyxml/inc)
include_directories(inc)

set(GAME_SRC
	src/main/main.cpp
	src/main/MyGame.cpp
	src/main/Rasterizer.cpp
	src/main/Rasterizer2.cpp
)

add_executable(game-main ${GAME_SRC})
target_link_libraries(game-main replacement-new memory-manager engine tinyxml glew ${COMMONS_LIB})
