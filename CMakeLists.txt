#########
# Setup #
#########

# Specify the minimum version for CMake
cmake_minimum_required(VERSION 2.8)

# Message
message(STATUS "CENG 5270 hw3")
message(STATUS ${CMAKE_CURRENT_SOURCE_DIR})
message(STATUS ${PROJECT_SOURCE_DIR})

# Find includes in the build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Project's name
project(hw3)

# Set the output folder where your program will be created
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin/)
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})

set(CMAKE_CXX_STANDARD 14)

# set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3")
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")

set(PATH_SRC ${PROJECT_SOURCE_DIR}/src/)

###################
# Warnings/Errors #
###################

set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Werror=return-type")

######################
# Check Dependencies #
######################
# uncommment this line if you are using self-compiled boost lib
# set(Boost_USE_STATIC_LIBS ON)
find_package(Boost COMPONENTS system filesystem program_options REQUIRED)
# find_package(Threads)

###############
# Source Code #
###############
file(GLOB_RECURSE SRC_FILES
    ${PATH_SRC}/*.cpp
    ${PATH_SRC}/*.cc
    ${PATH_SRC}/*.c)

###########
# Targets #
###########

add_executable(hw3 ${SRC_FILES})

#######################
# Include Directories #
#######################

include_directories(${PATH_SRC})

#################
# Linker Flags  #
#################

# TODO: set static under release mode only
set_target_properties(hw3 PROPERTIES LINK_FLAGS "-static -Wl,--whole-archive -rdynamic -lpthread -Wl,--no-whole-archive")

# Boost
target_include_directories(hw3 PUBLIC ${Boost_INCLUDE_DIR})
target_link_libraries(hw3 ${Boost_LIBRARIES})
