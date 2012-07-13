cmake_minimum_required(VERSION 2.6)
project(GTest CXX C)

#sources
set(GTEST_SRC
	src/gtest-all.cc
)

include_directories(.)
include_directories(inc)

add_library(gtest ${GTEST_SRC})
