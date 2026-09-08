cmake_minimum_required(VERSION 3.16)

project(TrongDongPalace LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

file(GLOB PROJECT_SOURCES CONFIGURE_DEPENDS "src/*.cpp")

add_executable(TrongDongPalace main.cpp ${PROJECT_SOURCES})
target_include_directories(TrongDongPalace PRIVATE include)
