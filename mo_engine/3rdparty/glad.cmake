file(GLOB glad_source ${CMAKE_CURRENT_SOURCE_DIR}/glad-c/src/glad.c)

add_library(glad ${glad_source})
target_include_directories(glad PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/glad-c/include)

