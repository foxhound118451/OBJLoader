add_executable(OBJViewer)
target_sources (OBJViewer
   PRIVATE
      src/gl.c
      src/model.cpp
      src/test.cpp
)
target_include_directories(OBJViewer
   PRIVATE
      ../include
      src
)
target_link_libraries(OBJViewer
   PRIVATE
      glfw
      glm
      objloader
)
