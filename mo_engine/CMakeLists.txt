set(ENGINE_ROOT_DIR "${MO_ROOT_DIR}/engine")
set(THIRD_PARTY_DIR "${ENGINE_ROOT_DIR}/3rdparty")
set(GA_ASSET_DIR "/asset")
set(GA_SHADER_DIR "/shader")

if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    add_compile_options("/W3")
endif()

add_subdirectory(3rdparty)

# add_subdirectory(shader)

add_subdirectory(example)

# add_subdirectory(source)


