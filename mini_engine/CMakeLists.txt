set(TARGET_NAME "mini_engine")
set(MODEL_NAME ${TARGET_NAME})

set(MINIENGINE_ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
set(THIRD_PARTY_DIR "${MINIENGINE_ROOT_DIR}/3rdparty")
set(MINIENGINE_ASSET_DIR "${MINIENGINE_ROOT_DIR}/asset")
set(MINIENGINE_SHADER_DIR "${MINIENGINE_ROOT_DIR}/shader")
set(MINIENGINE_BINARY_DIR "${MINIENGINE_ROOT_DIR}/bin")

add_custom_target("${TARGET_NAME}" ALL COMMAND "pwd")
set_target_properties("${TARGET_NAME}" PROPERTIES FOLDER "${TARGET_NAME}" )

add_subdirectory(source)
add_subdirectory(3rdparty)
