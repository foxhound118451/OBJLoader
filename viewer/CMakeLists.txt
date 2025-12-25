add_executable(OBJViewer)
target_sources (OBJViewer
   PRIVATE
      src/gl.c
      src/model.cpp
      src/stb_image.cpp
      src/viewer.cpp
)
target_include_directories(OBJViewer
   PRIVATE
      ../include
      src
)
find_package(nanogui)
target_link_libraries(OBJViewer
   PRIVATE
      glfw
      glm
      nanogui
      objloader
)
add_custom_command(
    TARGET OBJViewer POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/shaders
            ${CMAKE_CURRENT_BINARY_DIR}/shaders)
add_custom_command(
    TARGET OBJViewer POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/models
            ${CMAKE_CURRENT_BINARY_DIR}/models)
