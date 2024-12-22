cmake_minimum_required(VERSION 3.5.0)
project(2s2a VERSION 0.1.0 LANGUAGES C CXX)
cmake_policy(SET CMP0072 NEW)

find_package(OpenGL REQUIRED)

add_executable(2s2a src/main.cpp 
src/third-party/glad/include/glad/glad.h 
src/third-party/glad/src/glad.c
src/include/App.hpp
src/App.cpp
)

target_link_libraries(2s2a glfw)

