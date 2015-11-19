# Notes for CMake:
# You may need to extend your path / library path for cmake to include directories which contain GLEW or other libraries,
# e.g.: 
# set(CMAKE_PREFIX_PATH "d:/libs/glew-1.10.0")
# set(CMAKE_LIBRARY_PATH "d:/libs/glew-1.10.0/lib/Release/Win32/")
# or using your GUI

cmake_minimum_required(VERSION 2.8.11)

project (BambooEngine)

set(INCLUDE_DIR "include")

add_subdirectory(external/glfw)
include_directories(external/glfw/include)

# Add paths for GLEW
find_package(GLEW REQUIRED)
include_directories(${GLEW_INCLUDE_DIR})

# Add source/header files
file (GLOB HEADER_FILES "${INCLUDE_DIR}/*.h")
aux_source_directory(src SOURCE_FILES)
source_group("Header Files" FILES ${HEADER_FILES})



# Make sure we get C++11
SET(ENABLE_CXX11 "-std=c++1y")
if(CMAKE_COMPILER_IS_GNUCXX)
   EXECUTE_PROCESS(COMMAND "${CMAKE_CXX_COMPILER} -dumpversion" OUTPUT_VARIABLE GCC_VERSION)
   if (GCC_VERSION VERSION_LESS 4.7)
      SET(ENABLE_CXX11 "-std=c++0x")
   endif()
endif()
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${ENABLE_CXX11}")

#SET_TARGET_PROPERTIES(BambooEngine PROPERTIES DEBUG_POSTFIX "_d")
add_executable(BambooEngine ${SOURCE_FILES})

target_link_libraries(BambooEngine glfw ${GLFW_LIBRARIES} ${GLEW_LIBRARY})

add_custom_command(TARGET BambooEngine POST_BUILD        # Adds a post-build event to MyTest
    COMMAND ${CMAKE_COMMAND} -E copy_if_different  # which executes "cmake - E copy_if_different..."
        "${GLEW_INCLUDE_DIR}/../bin/Release/x64/glew32.dll"      # <--this is in-file
        $<TARGET_FILE_DIR:BambooEngine>)                 # <--this is out-file path

# include test subdirectory
#add_subdirectory("test")

# add tests to cmake project, so that it generates a ctest target
#enable_testing()
#add_test(tests test/tests)

