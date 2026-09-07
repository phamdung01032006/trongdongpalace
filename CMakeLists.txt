cmake_minimum_required(VERSION 3.10)
# Thêm chữ C vào phần ngôn ngữ
project(TrongDongPalace VERSION 0.1.0 LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(TrongDongPalace
    src/main.cpp
    src/database/DatabaseManager.cpp
    src/database/DatabaseInitializer.cpp
    src/repositories/CustomerRepository.cpp
    src/repositories/HallRepository.cpp
    src/repositories/EventRepository.cpp
    src/services/CustomerService.cpp
    src/services/HallService.cpp
    src/services/EventService.cpp
    src/sqlite3.c
    src/utils/DisplayHelper.cpp
    src/utils/InputHelper.cpp
    src/ui/MainMenu.cpp
    src/ui/ConsoleUI.cpp
    src/ui/CustomerMenu.cpp
    src/ui/HallMenu.cpp
    src/ui/EventMenu.cpp
)

# Giúp các file .cpp tìm thấy sqlite3.h dễ dàng hơn
target_include_directories(TrongDongPalace PRIVATE src)
