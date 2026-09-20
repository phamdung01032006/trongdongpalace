cmake_minimum_required(VERSION 3.10)
project(EventScheduling)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(include)

add_executable(event_scheduling
    src/main.cpp
    src/Event.cpp
    src/WeddingEvent.cpp
    src/ConferenceEvent.cpp
    src/BirthdayEvent.cpp
    src/Hall.cpp
    src/BookingRequest.cpp
    src/HeThongDatLich.cpp
    src/Utils.cpp
)
