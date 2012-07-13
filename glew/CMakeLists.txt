cmake_minimum_required(VERSION 2.6)
project(Glew CXX C)

#sources
set(GLEW_SRC
	src/glew.c
)

add_definitions(-DGLEW_STATIC)

include_directories(inc)
add_library(glew ${GLEW_SRC})
