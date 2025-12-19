cmake_minimum_required (VERSION 3.23)
set (CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

project (OBJLoader)

set(CMAKE_BUILD_TYPE Debug)

add_subdirectory(loader)
add_subdirectory(viewer)
