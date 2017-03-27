# Notes for CMake:
# You may need to extend your path / library path for cmake to include directories which contain GLEW or other libraries,
# e.g.: 
# set(CMAKE_PREFIX_PATH "d:/libs/glew-1.10.0")
# set(CMAKE_LIBRARY_PATH "d:/libs/glew-1.10.0/lib/Release/Win32/")
# or using your GUI

cmake_minimum_required(VERSION 3.6)

#set(CMAKE_SUPPRESS_REGENERATION true)

project (BambooEngine)

add_subdirectory(external/glfw)
include_directories(external/glfw/include)

include_directories(${BAMBOO_LIB}/include)
include_directories("include/")
include_directories("src/")

## Add googletest
# Download and unpack googletest at configure time
configure_file(CMakeLists.txt.googletest.in googletest-download/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/googletest-download )
if(result)
  message(FATAL_ERROR "CMake step for googletest failed: ${result}")
endif()
execute_process(COMMAND ${CMAKE_COMMAND} --build .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/googletest-download )
if(result)
  message(FATAL_ERROR "Build step for googletest failed: ${result}")
endif()

# Prevent overriding the parent project's compiler/linker
# settings on Windows
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# Add googletest directly to our build. This defines
# the gtest and gtest_main targets.
add_subdirectory(${CMAKE_BINARY_DIR}/googletest-src
                 ${CMAKE_BINARY_DIR}/googletest-build)
enable_testing()

################################
# Unit Tests
################################
# Add test cpp file
add_executable( runUnitTests tests/testgtest.cpp tests/TestNodes_basics.cpp tests/TestNodes.cpp src/Nodes/nodes.cpp)
# Link test executable against gtest & gtest_main
target_link_libraries(runUnitTests gtest gtest_main)
add_test(runUnitTests runUnitTests)

## Add boost
if (WIN32)
  set(BOOST_ROOT "D:/Development/Common-Externals/boost_1_63_0_msvc140-x64/")
  set(Boost_LIBRARY_DIRS "D:/Development/Common-Externals/boost_1_63_0_msvc140-x64/lib64-msvc-14.0")
endif()

set(Boost_USE_STATIC_LIBS OFF) 
set(Boost_USE_MULTITHREADED ON)  
set(Boost_USE_STATIC_RUNTIME OFF) 
find_package(Boost 1.60.0 REQUIRED COMPONENTS system)

if(Boost_FOUND)
  include_directories(${Boost_INCLUDE_DIRS})
  link_directories(${Boost_LIBRARY_DIRS})
else()
  message(FATAL_ERROR "Could not find boost." )
endif()

if (NOT WIN32)
    # Add paths for GL
    find_package(OpenGL REQUIRED)
    include_directories(${OPENGL_INCLUDE_DIRS})
    set(OPENGL_LINK_LIBRARIES ${OPENGL_LIBRARIES})
    #message( FATAL_ERROR "You can not do this at all, CMake will exit." )
else()
    set(OPENGL_LINK_LIBRARIES "opengl32")
endif()

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

#MESSAGE("DBG ALL_FILES=${ALL_FILES}")

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


# Make sure we get C++11
SET(ENABLE_CXX11 "-std=c++1y")
if(CMAKE_COMPILER_IS_GNUCXX)
   EXECUTE_PROCESS(COMMAND "${CMAKE_CXX_COMPILER} -dumpversion" OUTPUT_VARIABLE GCC_VERSION)
   if (GCC_VERSION VERSION_LESS 4.7)
      SET(ENABLE_CXX11 "-std=c++0x")
   endif()
endif()
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")


#SET_TARGET_PROPERTIES(BambooEngine PROPERTIES DEBUG_POSTFIX "_d")
add_executable(BambooEngine ${ALL_FILES_f} ${HEADER_FILES})

target_link_libraries(BambooEngine glfw ${OPENGL_LINK_LIBRARIES} ${GLFW_LIBRARIES} ${GLEW_LIBRARY} ${Boost_LIBRARIES})

if (WIN32)
    add_custom_command(TARGET BambooEngine POST_BUILD        # Adds a post-build event to MyTest
        COMMAND ${CMAKE_COMMAND} -E copy_if_different  # which executes "cmake - E copy_if_different..."
            "${GLEW_INCLUDE_DIR}/../bin/Release/x64/glew32.dll"      # <--this is in-file
            $<TARGET_FILE_DIR:BambooEngine>)                 # <--this is out-file path
endif()
