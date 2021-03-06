# - Find libev
# Find the native LLVM includes and library
#
# LLVM_INCLUDE_DIR - where to find ev.h, etc.
# LLVM_LIBRARIES - List of libraries when using libev.
# LLVM_FOUND - True if libev found.

find_program(LLVM_CONFIG_EXECUTABLE NAMES llvm-config DOC "llvm-config executable")

execute_process(
	COMMAND ${LLVM_CONFIG_EXECUTABLE} --cppflags
	OUTPUT_VARIABLE LLVM_CPPFLAGS
	OUTPUT_STRIP_TRAILING_WHITESPACE
)

execute_process(
	COMMAND ${LLVM_CONFIG_EXECUTABLE} --cxxflags
	OUTPUT_VARIABLE LLVM_CXXFLAGS
	OUTPUT_STRIP_TRAILING_WHITESPACE
)

execute_process(
	COMMAND ${LLVM_CONFIG_EXECUTABLE} --ldflags
	OUTPUT_VARIABLE LLVM_LDFLAGS
	OUTPUT_STRIP_TRAILING_WHITESPACE
)

execute_process(
	COMMAND ${LLVM_CONFIG_EXECUTABLE} --libs ${LLVM_COMPONENTS}
	OUTPUT_VARIABLE LLVM_LIBS
	OUTPUT_STRIP_TRAILING_WHITESPACE
)

execute_process(
	COMMAND ${LLVM_CONFIG_EXECUTABLE} --libs all
	OUTPUT_VARIABLE LLVM_ALL_LIBS
	OUTPUT_STRIP_TRAILING_WHITESPACE
)

execute_process(
	COMMAND ${LLVM_CONFIG_EXECUTABLE} --includedir
	OUTPUT_VARIABLE LLVM_INCLUDE_DIR
	OUTPUT_STRIP_TRAILING_WHITESPACE
)

execute_process(
	COMMAND ${LLVM_CONFIG_EXECUTABLE} --libdir
	OUTPUT_VARIABLE LLVM_LIB_DIR
	OUTPUT_STRIP_TRAILING_WHITESPACE
)

string(REGEX MATCHALL "-l[^ ]+" TEMP "${LLVM_LDFLAGS}")
string(REGEX REPLACE ";" " " TEMP1 "${TEMP}")

set(LLVM_LIBS "${LLVM_LIBS} ${TEMP1}")
