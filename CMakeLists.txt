cmake_minimum_required(VERSION 3.16)

project(TrongDongPalace LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

file(GLOB PROJECT_SOURCES CONFIGURE_DEPENDS "src/*.cpp")

add_executable(TrongDongPalace main.cpp ${PROJECT_SOURCES})
target_include_directories(TrongDongPalace PRIVATE include third_party)

# Make the executable runnable from a normal Windows terminal even when a
# different MinGW runtime happens to appear earlier in PATH.
if(MINGW)
    target_link_options(TrongDongPalace PRIVATE -static-libgcc -static-libstdc++)
endif()

include(CTest)
if(BUILD_TESTING)
    add_executable(FoundationTests tests/FoundationTests.cpp ${PROJECT_SOURCES})
    target_include_directories(FoundationTests PRIVATE include third_party)
    if(MINGW)
        target_link_options(FoundationTests PRIVATE -static-libgcc -static-libstdc++)
    endif()
    add_test(NAME FoundationTests COMMAND FoundationTests)
    add_test(NAME ExceptionHierarchyTests COMMAND FoundationTests exceptions)
    add_test(NAME JsonStorageTests COMMAND FoundationTests json)
    add_test(NAME BaseModelTests COMMAND FoundationTests models)
    add_test(NAME EventPolymorphismTests COMMAND FoundationTests events)
    add_test(NAME CustomerCrudTests COMMAND FoundationTests customer)
    add_test(NAME CenterHallCrudTests COMMAND FoundationTests centerhall)
    add_test(NAME AuthenticationAuthorizationTests COMMAND FoundationTests auth)
endif()
