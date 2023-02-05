function(start_demo targetName) 

set(TARGET_NAME ${targetName})
file(GLOB HEADERS CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/*.h)
file(GLOB SOURCES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

add_executable(${TARGET_NAME} ${HEADERS} ${SOURCES})

target_link_libraries(${TARGET_NAME} PUBLIC ${ARGN})

set_target_properties(${TARGET_NAME} PROPERTIES FOLDER "graphical_api/OpenGL")

set(POST_BUILD_COMMANDS
    COMMAND ${CMAKE_COMMAND} -E copy_directory "$<TARGET_FILE_DIR:${TARGET_NAME}>/" "${GA_BINARY_DIR}"
)

add_custom_command(TARGET ${TARGET_NAME} ${POST_BUILD_COMMANDS})

endfunction()
