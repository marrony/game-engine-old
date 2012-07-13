cmake_minimum_required(VERSION 2.6)
project(TinyXml CXX C)

#sources
set(TINYXML_SRC
	src/tinystr.cpp
	src/tinyxml.cpp
	src/tinyxmlerror.cpp
	src/tinyxmlparser.cpp
)

include_directories(inc)
add_library(tinyxml ${TINYXML_SRC})
