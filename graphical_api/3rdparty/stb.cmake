file(GLOB std_source CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/stb/*.h")
add_library(stb INTERFACE ${std_source})
target_include_directories(stb INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/stb)