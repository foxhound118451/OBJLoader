add_library(objloader)
target_sources(objloader
    PRIVATE
        src/material.cpp
        src/model_data.cpp
)
target_include_directories(objloader
    PUBLIC
        src
    PRIVATE
        ../include
)
target_link_libraries(objloader
    PRIVATE
        glm
)


