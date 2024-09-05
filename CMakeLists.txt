cmake_minimum_required(VERSION 3.10)

project(gymdeck2 VERSION 1.0)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED True)

set(SOURCES src/main.c)

find_path(RYZENADJ_INCLUDE_DIR
  NAMES ryzenadj.h
  HINTS ${CMAKE_SOURCE_DIR}/include /usr/local/include /usr/include
)

find_library(RYZENADJ_LIBRARY
  NAMES ryzenadj
  HINTS ${CMAKE_SOURCE_DIR}/lib /usr/local/lib /usr/lib
)

if(NOT RYZENADJ_LIBRARY)
  message(FATAL_ERROR "libryzenadj.so is not found.")
endif()

if(NOT RYZENADJ_INCLUDE_DIR)
  message(FATAL_ERROR "ryzenadj.h headers are not found.")
endif()

include_directories(${RYZENADJ_INCLUDE_DIR})

link_directories(${RYZENADJ_LIBRARY})

add_executable(gymdeck2 ${SOURCES})

target_link_libraries(gymdeck2 ${RYZENADJ_LIBRARY})

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
