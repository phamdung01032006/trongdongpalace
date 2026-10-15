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

find_package(Qt6 REQUIRED COMPONENTS Widgets)

# Theo yeu cau chi dung .cpp: main.cpp include cay ma nguon mot lan.
# Khong bien dich lai cac .cpp duoc include thanh translation unit rieng.
add_executable(trongdongpalace src/main.cpp)

target_include_directories(trongdongpalace PRIVATE src)
target_link_libraries(trongdongpalace PRIVATE
    Qt6::Widgets
)
target_compile_definitions(trongdongpalace PRIVATE TDP_PROJECT_DIR="${CMAKE_CURRENT_SOURCE_DIR}")

add_executable(tdp_tests tests/test_core.cpp)
target_include_directories(tdp_tests PRIVATE src)
enable_testing()
add_test(NAME core COMMAND tdp_tests)

add_executable(tdp_ui_tests tests/test_ui.cpp)
target_include_directories(tdp_ui_tests PRIVATE src)
target_link_libraries(tdp_ui_tests PRIVATE Qt6::Widgets)
add_test(NAME guest_ui COMMAND tdp_ui_tests)
set_tests_properties(guest_ui PROPERTIES ENVIRONMENT "QT_QPA_PLATFORM=offscreen" TIMEOUT 20)

if(MINGW)
    foreach(tgt trongdongpalace tdp_tests tdp_ui_tests)
        set_target_properties(${tgt} PROPERTIES
            LINK_FLAGS "-static -static-libgcc -static-libstdc++"
        )
    endforeach()
endif()
