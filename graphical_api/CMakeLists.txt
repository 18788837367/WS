set(TARGET_NAME "graphical_api")

set(GA_ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
set(THIRD_PARTY_DIR "${GA_ROOT_DIR}/3rdparty")
set(GA_ASSET_DIR "${GA_ROOT_DIR}/asset")
set(GA_SHADER_DIR "${GA_ROOT_DIR}/shader")
set(GA_BINARY_DIR "${GA_ROOT_DIR}/bin")

if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    add_compile_options("/W3")
endif()

add_custom_target("${TARGET_NAME}" ALL COMMAND  ${CMAKE_COMMAND} -E echo ${TARGET_NAME})
set_target_properties("${TARGET_NAME}" PROPERTIES FOLDER "${TARGET_NAME}" )

add_subdirectory(source)
add_subdirectory(3rdparty)

