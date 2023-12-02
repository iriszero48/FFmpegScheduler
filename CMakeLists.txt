cmake_minimum_required(VERSION 3.9)
project(FFmpegScheduler VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_VERBOSE_MAKEFILE ON)

file(GLOB_RECURSE CU_FILES "CppUtility/*.hpp")
file(GLOB_RECURSE SRC_FILES "FFmpegScheduler/*.hpp" "FFmpegScheduler/*.cpp")

add_executable(FFmpegScheduler ${SRC_FILES} ${CU_FILES})
target_include_directories(FFmpegScheduler PRIVATE "CppUtility")

if (MSVC)
    target_compile_definitions(FFmpegScheduler PRIVATE UNICODE UNICODE_)
    target_compile_options(FFmpegScheduler PRIVATE /MTd)
endif()