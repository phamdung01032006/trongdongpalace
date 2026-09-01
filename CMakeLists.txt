cmake_minimum_required(VERSION 3.10)
# Thêm chữ C vào phần ngôn ngữ
project(TrongDongPalace VERSION 0.1.0 LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(TrongDongPalace
    src/main.cpp
    src/database/DatabaseManager.cpp
    src/database/DatabaseInitializer.cpp
    src/sqlite3.c
)

# Giúp các file .cpp tìm thấy sqlite3.h dễ dàng hơn
target_include_directories(TrongDongPalace PRIVATE src)