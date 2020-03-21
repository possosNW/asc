cmake_minimum_required (VERSION 3.8)
project(ascLib_project VERSION 1.0.0 DESCRIPTION "Advanced Scene Change detection library")
# Default to `release` build if nothing was specified
SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=leak")
if(NOT CMAKE_BUILD_TYPE)
  message(STATUS "No build type selected, default to release")
  SET(CMAKE_BUILD_TYPE release)
endif()
set(CMAKE_CFLAGS "${CMAKE_CFLAGS} -m64 -fsanitize=leak")
include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
if(COMPILER_SUPPORTS_CXX11)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
else()
        message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
endif()
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/bin)
add_definitions (-march=core-avx2)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
add_library (asc STATIC
                src/asc_AVX2_impl.cpp
                src/asc_C_impl.cpp
                src/asc_common_opt.cpp
                src/asc.cpp
                src/asc_SSE4_impl.cpp
                src/IOfunctions.cpp
                src/MotionEstimationEngine.cpp
                src/tree.cpp
)
set_target_properties(asc PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION 1
    PUBLIC_HEADER include/asc.h)
target_include_directories(asc PRIVATE .)





