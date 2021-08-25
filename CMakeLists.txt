cmake_minimum_required(VERSION 3.16)

project(hvrt_renderer LANGUAGES C CXX)

file(GLOB_RECURSE SOURCES
        ./src/**.cpp
        ./src/**.hpp
        )

add_executable(${PROJECT_NAME} WIN32 ${SOURCES})
