cmake_minimum_required(VERSION 3.20)

project(TaskSched VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(tasksched
    src/console_logger.cpp
    src/edf_scheduler.cpp
    src/rms_scheduler.cpp
    src/scheduler_factory.cpp
    src/simulator.cpp
)

target_include_directories(tasksched PUBLIC include)
target_compile_features(tasksched PUBLIC cxx_std_20)

if(MSVC)
    target_compile_options(tasksched PRIVATE /W4 /permissive-)
else()
    target_compile_options(tasksched PRIVATE -Wall -Wextra -Wpedantic -Wconversion)
endif()

add_executable(tasksched_demo src/main.cpp)
target_link_libraries(tasksched_demo PRIVATE tasksched)

include(CTest)
if(BUILD_TESTING)
    add_executable(tasksched_tests tests/scheduler_tests.cpp)
    target_link_libraries(tasksched_tests PRIVATE tasksched)
    add_test(NAME scheduler_selection COMMAND tasksched_tests)
endif()
