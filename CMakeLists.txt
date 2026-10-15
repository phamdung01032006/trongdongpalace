cmake_minimum_required(VERSION 3.16)
project(TrongDongPalace LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(MSVC)
    add_compile_options(/utf-8 /W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(nlohmann_json 3.11.3 CONFIG QUIET)
if(NOT nlohmann_json_FOUND)
    include(FetchContent)

    FetchContent_Declare(
        nlohmann_json
        GIT_REPOSITORY https://github.com/nlohmann/json.git
        GIT_TAG        v3.11.3
    )
    FetchContent_MakeAvailable(nlohmann_json)
endif()

find_package(Qt6 REQUIRED COMPONENTS Widgets)

set(TDP_CORE_SOURCES
    src/utils/sha256.cpp
    src/utils/helpers.cpp
    src/models/Enums.cpp
    src/models/User.cpp
    src/models/Customer.cpp
    src/models/EventRequest.cpp
    src/models/Hall.cpp
    src/models/ServiceItem.cpp
    src/models/Booking.cpp
    src/models/Payment.cpp
    src/models/Event.cpp
    src/storage/json_storage.cpp
    src/services/palace_auth.cpp
    src/services/palace_requests.cpp
    src/services/palace_bookings.cpp
    src/services/palace_payments.cpp
)

add_executable(trongdongpalace
    src/main.cpp
    src/ui/app_ui.cpp
    ${TDP_CORE_SOURCES}
)

target_include_directories(trongdongpalace PRIVATE src)
target_link_libraries(trongdongpalace PRIVATE
    Qt6::Widgets
    nlohmann_json::nlohmann_json
)

add_executable(tdp_tests
    tests/test_core.cpp
    ${TDP_CORE_SOURCES}
)
target_include_directories(tdp_tests PRIVATE src)
target_link_libraries(tdp_tests PRIVATE nlohmann_json::nlohmann_json)

if(MINGW)
    foreach(tgt trongdongpalace tdp_tests)
        set_target_properties(${tgt} PROPERTIES
            LINK_FLAGS "-static -static-libgcc -static-libstdc++"
        )
    endforeach()
endif()
