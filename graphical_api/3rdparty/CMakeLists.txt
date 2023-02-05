set(third_party_folder "ThirdParty")

set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

if(NOT TARGET glfw)
    option(GLFW_BUILD_EXAMPLES "" OFF)
    option(GLFW_BUILD_TESTS "" OFF)
    option(GLFW_BUILD_DOCS "" OFF)
    option(GLFW_INSTALL "" OFF)
    add_subdirectory(glfw-3.3.8)
    set_target_properties(glfw PROPERTIES FOLDER ${third_party_folder}/glfw)
    set_target_properties(update_mappings PROPERTIES FOLDER ${third_party_folder}/glfw)
endif()

if(NOT TARGET assimp)
    add_subdirectory(assimp-5.2.5)
    set_target_properties(assimp PROPERTIES FOLDER ${third_party_folder}/assimp)
endif()

if(NOT TARGET glad)
    include(glad.cmake)
    set_target_properties(glad PROPERTIES FOLDER ${third_party_folder}/glad)
endif()

if(NOT TARGET glm)
    option(BUILD_STATIC_LIBS "" ON)
    add_subdirectory(glm-0.9.9.8)
    set_target_properties(glm_static PROPERTIES FOLDER ${third_party_folder}/glm)
endif()

if(NOT TARGET stb) 
    include(stb.cmake)
    set_target_properties(stb PROPERTIES FOLDER ${third_party_folder}/stb)
endif()