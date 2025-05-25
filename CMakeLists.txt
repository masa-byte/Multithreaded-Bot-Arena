# CMakeList.txt : CMake project for Project, include source and define
# project specific logic here.
#

# Add source to this project's executable.
cmake_minimum_required(VERSION 3.10)

project ("Project")

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(APPLE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_Alignof=alignof")
endif()

add_definitions(-DSOURCE_DIR="${CMAKE_SOURCE_DIR}")

add_executable (Project 
"Project.cpp" "Project.h" 
"item.h"  
"bot.h" "bot.cpp" 
"arena.h" "arena.cpp" 
"utils.h" "utils.cpp"
 "item.cpp")


# TODO: Add tests and install targets if needed.
