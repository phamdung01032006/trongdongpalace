cmake_minimum_required(VERSION 3.16)
project(TrongDongPalace LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# /utf-8 giup MSVC doc dung tieng Viet trong source code.
if(MSVC)
    add_compile_options(/utf-8 /W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

add_executable(trongdongpalace
    main.cpp
    src/Utils.cpp
    src/DateTime.cpp
    src/Enums.cpp
    src/Person.cpp
    src/Customer.cpp
    src/User.cpp
    src/Hall.cpp
    src/Service.cpp
    src/EventRequest.cpp
    src/Booking.cpp
    src/Payment.cpp
    src/Event.cpp
    src/Store.cpp
    src/PalaceService.cpp
    src/PalaceRequest.cpp
    src/PalaceBooking.cpp
    src/ConsoleUI.cpp
)

target_include_directories(trongdongpalace PRIVATE include)

# Tren MinGW: nhung san thu vien C++ de copy .exe sang may khac de chay.
if(MINGW)
    target_link_options(trongdongpalace PRIVATE -static -static-libgcc -static-libstdc++)
endif()
