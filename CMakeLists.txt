cmake_minimum_required(VERSION 3.10)
project(TrongDongPalace VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SQLite3 REQUIRED)

# Từ Step tiếp theo (Exceptions/Repositories/...), thêm file .cpp mới
# vào danh sách này khi tạo ra.
add_executable(TrongDongPalace
    src/main.cpp
    src/database/DatabaseManager.cpp
)

target_include_directories(TrongDongPalace PRIVATE src ${SQLite3_INCLUDE_DIRS})
target_link_libraries(TrongDongPalace PRIVATE ${SQLite3_LIBRARIES})
