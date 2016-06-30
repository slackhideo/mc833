cmake_minimum_required(VERSION 3.0.2) # CMake version check
project(projeto)               # Create project "simple_example"
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread") # Enable c++11 standard

set(CLIENT_FILES projeto/client/client.cpp)            # Add main.cpp file of project root directory as source file
set(SERVER_FILES projeto/server/server.cpp projeto/classes/User.cpp projeto/classes/Message.cpp projeto/classes/Group.cpp)

add_executable(client ${CLIENT_FILES})       # Add executable target with source files listed in SOURCE_FILES variable
add_executable(server ${SERVER_FILES})