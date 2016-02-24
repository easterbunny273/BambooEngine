# Notes for CMake:
# You may need to extend your path / library path for cmake to include directories which contain GLEW or other libraries,
# e.g.: 
# set(CMAKE_PREFIX_PATH "d:/libs/glew-1.10.0")
# set(CMAKE_LIBRARY_PATH "d:/libs/glew-1.10.0/lib/Release/Win32/")
# or using your GUI

cmake_minimum_required(VERSION 2.8.11)

project (BambooEngine)

add_subdirectory(external/glfw)
include_directories(external/glfw/include)

include_directories(${BAMBOO_LIB}/include)
include_directories("include/")
include_directories("src/")

# Add paths for GLEW
find_package(GLEW REQUIRED)
include_directories(${GLEW_INCLUDE_DIR})

# Add source/header files
#file (GLOB HEADER_FILES "include/*.h")
#aux_source_directory(src SOURCE_FILES)
#source_group("Header Files" FILES ${HEADER_FILES})

set (SOURCE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/src)
file(GLOB_RECURSE ALL_FILES RELATIVE ${SOURCE_DIRECTORY} "src/*.cpp" "src/*.h")
file(GLOB_RECURSE ALL_FILES_f ${SOURCE_DIRECTORY} "src/*.cpp" "src/*.h")

MESSAGE("DBG ALL_FILES=${ALL_FILES}")

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

  MESSAGE("DBG GROUP=${GROUP}, FOO=${FILE}, ${FULL_FILENAME}")

  source_group("Source Files\\${GROUP}" FILES "${FULL_FILENAME}")
endforeach()


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
add_executable(BambooEngine ${ALL_FILES_f} ${HEADER_FILES})

target_link_libraries(BambooEngine glfw opengl32 ${GLFW_LIBRARIES} ${GLEW_LIBRARY})

add_custom_command(TARGET BambooEngine POST_BUILD        # Adds a post-build event to MyTest
    COMMAND ${CMAKE_COMMAND} -E copy_if_different  # which executes "cmake - E copy_if_different..."
        "${GLEW_INCLUDE_DIR}/../bin/Release/x64/glew32.dll"      # <--this is in-file
        $<TARGET_FILE_DIR:BambooEngine>)                 # <--this is out-file path
