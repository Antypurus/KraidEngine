cmake_minimum_required(VERSION 3.16)

project(KraidEngine LANGUAGES C CXX)
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
    "D3DCompiler.lib"
    "dxcompiler.lib"
)

target_include_directories(${PROJECT_NAME} PRIVATE ./src/)
target_precompile_headers(${PROJECT_NAME} PRIVATE [[<Core/Rendering/D3D12/D3D12.h>]])

add_compile_options(${PROJECT_NAME} /wd26495)

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)

get_property(dirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)
foreach(dir ${dirs})
  message(STATUS "dir='${dir}'")
endforeach()

add_custom_command(
    TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${PROJECT_SOURCE_DIR}/ThirdParty/DXC/"
    "${CMAKE_BINARY_DIR}")
