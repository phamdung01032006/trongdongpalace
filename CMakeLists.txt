cmake_minimum_required(VERSION 3.16)
project(TrongDongPalace LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
    add_compile_options(/utf-8 /W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

include(FetchContent)

FetchContent_Declare(
    ftxui
    GIT_REPOSITORY https://github.com/ArthurSonzogni/FTXUI.git
    GIT_TAG        v5.0.0
)
FetchContent_Declare(
    nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG        v3.11.3
)
FetchContent_MakeAvailable(ftxui nlohmann_json)

set(TDP_CORE_SOURCES
    src/utils/sha256.cpp
    src/utils/helpers.cpp
    src/models/entities.cpp
    src/storage/json_storage.cpp
    src/services/palace_service.cpp
)

add_executable(trongdongpalace
    src/main.cpp
    src/ui/app_ui.cpp
    ${TDP_CORE_SOURCES}
)

target_include_directories(trongdongpalace PRIVATE src)
target_link_libraries(trongdongpalace PRIVATE
    ftxui::screen
    ftxui::dom
    ftxui::component
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
