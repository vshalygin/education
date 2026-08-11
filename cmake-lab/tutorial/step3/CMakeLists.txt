cmake_minimum_required(VERSION 3.23)

project(Tutorial)

option(TUTORIAL_BUILD_UTILITIES "Build the Tutorial executable" ON)

if(TUTORIAL_BUILD_UTILITIES)
    add_subdirectory(Tutorial)
endif()

add_subdirectory(MathFunctions)
