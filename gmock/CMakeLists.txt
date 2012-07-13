cmake_minimum_required(VERSION 2.6)
project(GMock CXX C)

#sources
set(GMOCK_SRC
	src/gmock-all.cc
	src/gmock_main.cc
)

include_directories(.)
include_directories(inc)
include_directories(../gtest/inc)

add_library(gmock ${GMOCK_SRC})
