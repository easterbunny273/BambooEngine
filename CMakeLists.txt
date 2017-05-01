# Notes for CMake:
# You may need to extend your path / library path for cmake to include directories which contain GLEW or other libraries,
# e.g.: 
# set(CMAKE_PREFIX_PATH "d:/libs/glew-1.10.0")
# set(CMAKE_LIBRARY_PATH "d:/libs/glew-1.10.0/lib/Release/Win32/")
# or using your GUI

cmake_minimum_required(VERSION 3.6)

#set(CMAKE_SUPPRESS_REGENERATION true)

project (BambooEngine)

include_directories("include/")
include_directories("src/")

set (SOURCE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/src)
file(GLOB_RECURSE ALL_FILES RELATIVE ${SOURCE_DIRECTORY} "src/*.cpp" "src/*.h")
file(GLOB_RECURSE ALL_FILES_f ${SOURCE_DIRECTORY} "src/*.cpp" "src/*.h")

foreach(FILE ${ALL_FILES}) 
  get_filename_component(PARENT_DIR "${FILE}" PATH)

  set (FULL_FILENAME ${SOURCE_DIRECTORY}/${FILE})
  # skip src or include and changes /'s to \\'s
  string(REGEX REPLACE "(\\./)?(src|include)/?" "" GROUP "${PARENT_DIR}")
  string(REPLACE "/" "\\" GROUP "${GROUP}")

  # group into "Source Files" and "Header Files"
  #if ("${FILE}" MATCHES ".*\\.cpp")
  #  set(GROUP "Source Files\\${GROUP}")
  #elseif("${FILE}" MATCHES ".*\\.h")
  #  set(GROUP "Header Files\\${GROUP}")
  #endif()

  #MESSAGE("DBG GROUP=${GROUP}, FOO=${FILE}, ${FULL_FILENAME}")

  source_group("Source Files\\${GROUP}" FILES "${FULL_FILENAME}")
endforeach()



################################
# BambooEngine Project
################################

add_executable(BambooEngine ${ALL_FILES_f} ${HEADER_FILES})
set_property(TARGET BambooEngine PROPERTY CXX_STANDARD 17)
set_target_properties(BambooEngine PROPERTIES COMPILE_FLAGS "/std:c++latest")
#set_target_properties(BambooEngine PROPERTIES COMPILE_DEFINITIONS "_WIN32_WINDOWS")
set_target_properties(BambooEngine PROPERTIES COMPILE_DEFINITIONS " _WIN32_WINNT=0x0501")
 
include(external/glfw.cmake)
include(external/googletest.cmake)
include(external/boost.cmake)
include(external/glew.cmake)
include(external/opengl.cmake)

################################
# Unit Tests
################################

enable_testing()

# Add test cpp file
add_executable( runUnitTests tests/testgtest.cpp tests/TestNodes_basics.cpp tests/TestNodes.cpp src/Nodes/nodes.cpp)
set_property(TARGET runUnitTests PROPERTY CXX_STANDARD 17)
set_target_properties(runUnitTests PROPERTIES COMPILE_FLAGS "/std:c++latest")
set_target_properties(runUnitTests PROPERTIES COMPILE_DEFINITIONS "GTEST_LANG_CXX11")

# Link test executable against gtest & gtest_main
target_link_libraries(runUnitTests gtest gtest_main)
add_test(runUnitTests runUnitTests)

