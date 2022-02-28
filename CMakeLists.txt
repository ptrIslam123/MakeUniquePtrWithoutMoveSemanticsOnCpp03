cmake_minimum_required(VERSION 3.0.0)

project(UniquePtrWithoutMoveSemantic)

#add_definitions(-std=c++98)

# Locate GTest
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

# Link runTests with what we want to test and the GTest and pthread library
add_executable(main main.cpp)
target_link_libraries(main ${GTEST_LIBRARIES} pthread)
