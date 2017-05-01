## Add boost
if (WIN32)
  set(BOOST_ROOT "D:/Development/Common-Externals/boost_1_64_0")
  set(BOOST_LIBRARYDIR "D:/Development/Common-Externals/boost_1_64_0/lib64-msvc-14.1")
  set(Boost_LIBRARY_DIRS "D:/Development/Common-Externals/boost_1_64_0/lib64-msvc-14.1")
endif()

set(Boost_USE_STATIC_LIBS OFF) 
set(Boost_USE_MULTITHREADED ON)  
set(Boost_USE_STATIC_RUNTIME OFF) 
set(Boost_USE_DEBUG_RUNTIME OFF)

find_package(Boost 1.60.0 REQUIRED COMPONENTS system)

if(Boost_FOUND)
  include_directories(${Boost_INCLUDE_DIRS})
else()
  message(FATAL_ERROR "Could not find boost." )
endif()

target_link_libraries(BambooEngine ${Boost_LIBRARIES} Boost::disable_autolinking)

if (WIN32)
    add_custom_command(TARGET BambooEngine POST_BUILD        # Adds a post-build event to MyTest
        COMMAND ${CMAKE_COMMAND} -E copy_directory  # which executes "cmake - E copy_if_different..."
            "${BOOST_LIBRARYDIR}"      # <--this is in-file
            $<TARGET_FILE_DIR:BambooEngine>)                 # <--this is out-file path
endif()