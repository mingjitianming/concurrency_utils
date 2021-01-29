cmake_minimum_required(VERSION 3.12)
project(concurrency_utils
        LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

include(CheckCXXCompilerFlag)

check_cxx_compiler_flag("-std=c++20" COMPILER_SUPPORT_CXX20)
if(${COMPILER_SUPPORT_CXX20})
    set(CMAKE_CXX_STANDARD 20)
    set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
else()
    message(FATAL_ERROR "The compiler ${CMAKE_CXX_COMPILER} has no C++20 support. Please use a different C++ compiler.")
endif()

include_directories(
    include
)

# add_custom_target(fff SHARED "")
add_library(pool SHARED 
            src/stealing_queue.cpp)
target_link_libraries(pool
            INTERFACE
            pthread)

add_executable(containers 
            src/test.cpp)

target_link_libraries(containers
            pool)
