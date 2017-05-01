add_subdirectory(external/glfw)
include_directories(external/glfw/include)

target_link_libraries(BambooEngine glfw)