cmake_minimum_required(VERSION 3.16)

project(hvrt_renderer LANGUAGES C CXX)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS 
        ./src/**.cpp
        ./src/**.hpp
        ./src/**.h
        )

add_executable(${PROJECT_NAME} WIN32 ${SOURCES})

target_link_libraries(${PROJECT_NAME} PRIVATE
    "d3d12.lib"
    "dxgi.lib"
)

target_include_directories(${PROJECT_NAME} PRIVATE ./src/)

add_compile_options(${PROJECT_NAME} /MP)
add_compile_options(${PROJECT_NAME} /wd26495)

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)


