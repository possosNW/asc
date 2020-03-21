cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
# Above line indicates to Cmake that minimum version 2.8 is required.
# As you may have noted all lines beginning with hash symbol are treated as comments by Cmake.
include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
if(COMPILER_SUPPORTS_CXX11)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
else()
        message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
endif()
SET(appName ascApp)
# In the above line we declared a variable sampleName and assigned it a value MyApp. We will make use of the same later.project(PrjMyApp)
# here we define the project or solution (for visual studio) name
# In following lines we try to find out the packages of additional libraries, if reqd. Here we are trying to locate PCL and mrpt library.
# u may replace the names by yours. Each such package should have a Cmake file in them, if not then we may have to manually define the paths.
# we Show that later.
#find_package(PCL 1.2 REQUIRED)
#FIND_PACKAGE(MRPT REQUIRED base;bayes;obs;gui)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -msse4.1 -mavx")
# here we specify the additional include directories for the project. These files come in additional include directories option of VC++
# project.
# either the variable values from package like ${PCL_INCLUDE_DIRS} or absolute paths as shown in second and third line may be used.
#include_directories(${PCL_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR})
INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_SOURCE_DIR}/../ascLib )
LINK_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/../ascLib)

# here we specify the additional library directories for the linker in the project. These files come in additional library directories
# option of VC++ project.
# either the variable values from package like ${PCL_LIBRARY_DIRS} or absolute paths as shown in second and third line may be used.
# An update, link_directories is being pulled out, you may use TARGET_LINK_LIBRARIES instead as shown below
#link_directories(${PCL_LIBRARY_DIRS})

# here we add definitions for any package if requred.
#add_definitions(${PCL_DEFINITIONS})

# The following line is very important.
# It specifies the executable name. Here the executable is the first parameter i.e. MyApp and a file MyApp.exe will be created on
# compilation in windows platform.
# Followed by the executable name come all your source and header files.
# All cpp fles will be clubbed into source folder and all .h files will be added to header files folder of the project.
add_executable (ascApp ascApp.cpp argument.cpp )

# There may be some additional dependencies which you may have to specify for the project, which you may do as in following lines.
# Note that first parameter is the executable name.
target_link_libraries (ascApp asc )
# Add the required libraries for linking:
#TARGET_LINK_LIBRARIES(${sampleName}
#${MRPT_LIBS} # This is filled by FIND_PACKAGE(MRPT ...)
#"" # Optional extra libs...
#)
# End of CMakeLists.Txt file. Save it as a plain text file and the name should be exactly CMakeLists.Txt