cmake_minimum_required (VERSION 3.23)

project (OBJLoader)

add_executable (OBJLoader)

set(CMAKE_BUILD_TYPE Debug)

target_sources (OBJLoader
   PRIVATE
      src/viewer/gl.c
      src/viewer/model.cpp
      src/viewer/test.cpp
      src/loader/material.cpp
      src/loader/model_data.cpp
)
target_include_directories(OBJLoader
   PRIVATE
      include
      src/loader
      src/viewer
)
target_link_libraries(OBJLoader
   PRIVATE
      glfw
      glm
)

