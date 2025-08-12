cmake_minimum_required(VERSION 3.15)

project("g1_joystick")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "-O3 -pthread")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -pthread")

# Include the eigen headers
include_directories($ENV{G1_EIGEN_DIR})

# Include the header files
include_directories(include)

include_directories(${CMAKE_SOURCE_DIR}/include)

# Create a controller library
add_library(g1_joystick SHARED
    src/joy.cpp
)

# Link the libraries to the g1_controller library
target_link_libraries(g1_joystick)

# Include the directories in the g1_control library
target_include_directories(g1_joystick PUBLIC)


# Create an executable for debugging MPC functions
add_executable(xbox_controller_test tests/xbox_controller_test.cpp)
target_link_libraries(xbox_controller_test PUBLIC g1_joystick)
target_link_directories(xbox_controller_test PUBLIC
${CMAKE_SOURCE_DIR}/G1-Joystick/include)

add_executable(joy_mapping_example tests/joy_mapping_example.cpp)
target_link_libraries(joy_mapping_example PUBLIC g1_joystick)
target_link_directories(xbox_controller_test PUBLIC
${CMAKE_SOURCE_DIR}/G1-Joystick/include)