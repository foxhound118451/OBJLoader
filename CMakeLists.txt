cmake_minimum_required (VERSION 3.23)

project (OBJLoader)

add_executable (OBJLoader)

set(CMAKE_BUILD_TYPE Debug)

target_sources (OBJLoader
   PRIVATE
      src/gl.c
      src/test.cpp
      src/material.cpp
      src/model.cpp
)
target_include_directories(OBJLoader
   PRIVATE
      include
      src
)
target_link_libraries(OBJLoader
   PRIVATE
      glfw
      glm
)

