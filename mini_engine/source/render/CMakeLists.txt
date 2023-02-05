set(TARGET_NAME render)

file(GLOB_RECURSE HEADER_FILES "*.h")
file(GLOB_RECURSE SOURCE_FILES "*.cpp")

add_library(${TARGET_NAME} ${HEADER_FILES} ${SOURCE_FILES})

target_link_libraries(${TARGET_NAME} PRIVATE base)

target_link_libraries(${TARGET_NAME} PRIVATE "-framework Cocoa" "-framework OpenGL" "-framework CoreGraphics" "-framework CoreText")

target_include_directories(
    ${TARGET_NAME}
    PUBLIC $<BUILD_INTERFACE:${MINIENGINE_ROOT_DIR}/source/base>)

set_target_properties(${TARGET_NAME} PROPERTIES FOLDER "${MODEL_NAME}/${TARGET_NAME}")



